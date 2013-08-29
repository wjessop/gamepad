#include <xbox_1.h>

int perr(char const *format, ...) {
	va_list args;
	int r;

	va_start (args, format);
	r = vfprintf(stderr, format, args);
	va_end(args);

	return r;
}

#define ERR_EXIT(errcode) do { perr("   %s\n", libusb_strerror((enum libusb_error)errcode)); return -1; } while (0)
#define CALL_CHECK(fcall) do { r=fcall; if (r < 0) ERR_EXIT(r); } while (0);
#define B(x) (((x)!=0)?1:0)
#define be_to_int32(buf) (((buf)[0]<<24)|((buf)[1]<<16)|((buf)[2]<<8)|(buf)[3])

#define RETRY_MAX                     5
#define REQUEST_SENSE_LENGTH          0x12
#define INQUIRY_LENGTH                0x24
#define READ_CAPACITY_LENGTH          0x08

// HID Class-Specific Requests values. See section 7.2 of the HID specifications
#define HID_GET_REPORT                0x01
#define HID_GET_IDLE                  0x02
#define HID_GET_PROTOCOL              0x03
#define HID_SET_REPORT                0x09
#define HID_SET_IDLE                  0x0A
#define HID_SET_PROTOCOL              0x0B
#define HID_REPORT_TYPE_INPUT         0x01
#define HID_REPORT_TYPE_OUTPUT        0x02
#define HID_REPORT_TYPE_FEATURE       0x03


static int fetch_xbox_status(libusb_device_handle *handle, uint8_t input_report[20]);
static void loop_over_controller_status();

/*

	Ruby definitions

*/

VALUE method_on_changed(VALUE self);

void Init_xbox_1(VALUE module) {
	VALUE c_xbox_1;
	c_xbox_1 = rb_define_class_under(module, "Xbox1", rb_cObject);
	rb_define_method(c_xbox_1, "on_changed", method_on_changed, 0);
}

VALUE method_on_changed(VALUE self) {
	if (!rb_block_given_p())
		rb_raise(rb_eArgError, "Expected block");

	loop_over_controller_status();

	// Never get here
	return Qnil;
}

/*

	Regular c functions

*/

static bool run_usb_loop = true;

void signal_callback_handler(int signum) {
	run_usb_loop = false;
}

/*
	
	Yield a hash with the following data:

	D-pad
	Start
	Back
	Left Stick Press
	Right Stick Press
	A
	B
	X
	Y
	Black
	White
	Left Trigger
	Right Trigger
	Left Analog X
	Left Analog Y
	Right Analog X
	Right Analog Y

*/

static void loop_over_controller_status() {
	// The default context is OK, we're not going to use more than one context: http://libusb.sourceforge.net/api-1.0/contexts.html
	int ret = libusb_init(NULL);
	if (ret != LIBUSB_SUCCESS) {
		fprintf(stderr, "libusb_init returned %d\n", ret);
		exit(1);
	}
	
	// libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_DEBUG);

	libusb_device_handle* handle = libusb_open_device_with_vid_pid(NULL, 0x045e, 0x0289);
	
	int INPUT_REPORT_SIZE = 20;
	int SIZEOF_ARRAY = INPUT_REPORT_SIZE * sizeof(uint8_t);

	uint8_t input_report[INPUT_REPORT_SIZE];
	uint8_t old_input_report[INPUT_REPORT_SIZE];

	memset(input_report, 0, SIZEOF_ARRAY);
	memset(old_input_report, 0, SIZEOF_ARRAY);

	signal(SIGINT, signal_callback_handler);

	while (run_usb_loop) {
		fetch_xbox_status(handle, input_report);

		// Compare the old and new values, we only want to yield if the values change
		if (memcmp(input_report, old_input_report, SIZEOF_ARRAY) != 0) {

			// Store the new values for comparison next time round
			memcpy(old_input_report, input_report, SIZEOF_ARRAY);

			VALUE values = rb_hash_new();
			rb_hash_aset(values, ID2SYM(rb_intern("D-pad")), INT2NUM(input_report[2]&0x0F));
			rb_hash_aset(values, ID2SYM(rb_intern("Start")), INT2NUM(B(input_report[2]&0x10)));
			rb_hash_aset(values, ID2SYM(rb_intern("Back")), INT2NUM(B(input_report[2]&0x20)));
			rb_hash_aset(values, ID2SYM(rb_intern("Left Stick Press")), INT2NUM(B(input_report[2]&0x40)));
			rb_hash_aset(values, ID2SYM(rb_intern("Right Stick Press")), INT2NUM(B(input_report[2]&0x80)));

			// Presure sensitive, values from 0 to 255
			rb_hash_aset(values, ID2SYM(rb_intern("A")), INT2NUM(input_report[4]));
			rb_hash_aset(values, ID2SYM(rb_intern("B")), INT2NUM(input_report[5]));
			rb_hash_aset(values, ID2SYM(rb_intern("X")), INT2NUM(input_report[6]));
			rb_hash_aset(values, ID2SYM(rb_intern("Y")), INT2NUM(input_report[7]));
			rb_hash_aset(values, ID2SYM(rb_intern("Black")), INT2NUM(input_report[8]));
			rb_hash_aset(values, ID2SYM(rb_intern("White")), INT2NUM(input_report[9]));

			// Triggers are also pressure sensitive
			rb_hash_aset(values, ID2SYM(rb_intern("Left Trigger")), INT2NUM(input_report[10]));
			rb_hash_aset(values, ID2SYM(rb_intern("Right Trigger")), INT2NUM(input_report[11]));

			// Left Analog stick x/y
			rb_hash_aset(values, ID2SYM(rb_intern("Left Analog X")), INT2NUM((int16_t)((input_report[13]<<8)|input_report[12])));
			rb_hash_aset(values, ID2SYM(rb_intern("Left Analog Y")), INT2NUM((int16_t)((input_report[15]<<8)|input_report[14])));

			// Right Analog stick x/y
			rb_hash_aset(values, ID2SYM(rb_intern("Right Analog X")), INT2NUM((int16_t)((input_report[17]<<8)|input_report[16])));
			rb_hash_aset(values, ID2SYM(rb_intern("Right Analog Y")), INT2NUM((int16_t)((input_report[19]<<8)|input_report[18])));
			rb_yield(values);
		}

		// Sleep for a while
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		select(0, NULL, NULL, NULL, &tv);
	}
	
	libusb_close(handle);
	libusb_exit(NULL);
}

static int fetch_xbox_status(libusb_device_handle *handle, uint8_t input_report[20]) {
	int r;
	CALL_CHECK(libusb_control_transfer(handle, LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,HID_GET_REPORT, (HID_REPORT_TYPE_INPUT<<8)|0x00, 0, input_report, 20, 1000));
	return 0;
}
