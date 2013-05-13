#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
		
#define MY_UUID { 0x82, 0x98, 0x17, 0xE4, 0x61, 0xCD, 0x4D, 0x10, 0x89, 0xCB, 0x99, 0x52, 0xFD, 0xA8, 0xA7, 0x6A }
PBL_APP_INFO(MY_UUID,
             "RadarClock", "8a22a - Code, Elzor - Design",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

void upcase(char *str)
{
    for (int i = 0; str[i] != 0; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 0x20;
        }
    }
}

Window window;

TextLayer day_layer;
GFont day_font;
static char day_text[] ="SUN";

TextLayer date_layer;
GFont date_font;
static char date_text[] = "12";

TextLayer month_layer;
GFont month_font;
static char month_text[] = "MAY";

BmpContainer background_image_container;

RotBmpPairContainer minute_hand_image_container;
RotBmpPairContainer hour_hand_image_container;

void update_watch(PblTm* t){

  minute_hand_image_container.layer.white_layer.rotation = TRIG_MAX_ANGLE * (t->tm_min * 6) / 360;
  minute_hand_image_container.layer.black_layer.rotation = TRIG_MAX_ANGLE * (t->tm_min * 6) / 360;
  minute_hand_image_container.layer.layer.frame.origin.x = (72) - (minute_hand_image_container.layer.layer.frame.size.w/2);
  minute_hand_image_container.layer.layer.frame.origin.y = (75) - (minute_hand_image_container.layer.layer.frame.size.h/2);
  layer_mark_dirty(&minute_hand_image_container.layer.layer);	

  hour_hand_image_container.layer.white_layer.rotation = TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 30) + (t->tm_min/2)) / 360;
  hour_hand_image_container.layer.black_layer.rotation = TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 30) + (t->tm_min/2)) / 360;
  hour_hand_image_container.layer.layer.frame.origin.x = (72) - (hour_hand_image_container.layer.layer.frame.size.w/2);
  hour_hand_image_container.layer.layer.frame.origin.y = (75) - (hour_hand_image_container.layer.layer.frame.size.h/2);
  layer_mark_dirty(&hour_hand_image_container.layer.layer);

}

void draw_day(){
  PblTm t;
  get_time(&t);
  string_format_time(day_text, sizeof(day_text), "%a", &t);
  upcase((char*)(&day_text));
  text_layer_set_text(&day_layer, day_text);
}

void draw_date(){
  PblTm t;
  get_time(&t);
  string_format_time(date_text, sizeof(date_text), "%d", &t);
  text_layer_set_text(&date_layer, date_text);
}

void draw_month(){
  PblTm t;
  get_time(&t);
  string_format_time(month_text, sizeof(month_text), "%b", &t);
  upcase((char*)(&month_text));
  text_layer_set_text(&month_layer, month_text);
}

// Called once per second
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  update_watch(t->tick_time);
}

// Handle the start-up of the app
void handle_init(AppContextRef app_ctx) {

  // Create our app's base window
  window_init(&window, "Radar Clock");
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);

  // Set up a layer for the static watch face background
  bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image_container);
  layer_add_child(&window.layer, &background_image_container.layer.layer);

  // Set up a layer for the minute hand with transparency, ensure the source image has a transparent area
  rotbmp_pair_init_container(RESOURCE_ID_IMAGE_MINUTE_HAND_WHITE, RESOURCE_ID_IMAGE_MINUTE_HAND_BLACK, &minute_hand_image_container);
  rotbmp_pair_layer_set_src_ic(&minute_hand_image_container.layer, GPoint(50, 55));
  layer_add_child(&window.layer, &minute_hand_image_container.layer.layer);	

  // Set up a layer for the hour hand with transparency, ensure the source image has a transparent area
  rotbmp_pair_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_WHITE, RESOURCE_ID_IMAGE_HOUR_HAND_BLACK, &hour_hand_image_container);
  rotbmp_pair_layer_set_src_ic(&hour_hand_image_container.layer, GPoint(35, 38));
  layer_add_child(&window.layer, &hour_hand_image_container.layer.layer);

  day_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PIXELMIX_10));
  text_layer_init(&day_layer, GRect(34, 150, 28, 12)); 
  text_layer_set_text_alignment(&day_layer, GTextAlignmentCenter);
  text_layer_set_text_color(&day_layer, GColorWhite);
  text_layer_set_background_color(&day_layer, GColorClear);
  text_layer_set_font(&day_layer, day_font);
  layer_add_child(&window.layer, &day_layer.layer);
  draw_day();

  date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PIXELMIX_10));
  text_layer_init(&date_layer, GRect(63, 150, 21, 12));
  text_layer_set_text_alignment(&date_layer, GTextAlignmentCenter);
  text_layer_set_text_color(&date_layer, GColorWhite);
  text_layer_set_background_color(&date_layer, GColorClear);
  text_layer_set_font(&date_layer, date_font);
  layer_add_child(&window.layer, &date_layer.layer);
  draw_date();	

  month_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PIXELMIX_10));
  text_layer_init(&month_layer, GRect(84, 150, 28, 12));
  text_layer_set_text_alignment(&month_layer, GTextAlignmentCenter);
  text_layer_set_text_color(&month_layer, GColorWhite);
  text_layer_set_background_color(&month_layer, GColorClear);
  text_layer_set_font(&month_layer, month_font);
  layer_add_child(&window.layer, &month_layer.layer);
  draw_month();
	
  PblTm t;
  get_time(&t);
  update_watch(&t);
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  bmp_deinit_container(&background_image_container);
  rotbmp_pair_deinit_container(&minute_hand_image_container);
  rotbmp_pair_deinit_container(&hour_hand_image_container);
	
  fonts_unload_custom_font(day_font);
  fonts_unload_custom_font(date_font);
  fonts_unload_custom_font(month_font);
}

void handle_tick(AppContextRef ctx, PebbleTickEvent *t){
  (void)t;
  (void)ctx;

   if(t->tick_time->tm_min==0&&t->tick_time->tm_hour==0)
     {
     draw_day();
     draw_date();
     draw_month();
     }        
}

// The main event/run loop for our app
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {

    // Handle app start
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
	}
   };
  app_event_loop(params, &handlers);
}