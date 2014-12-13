#include <pebble.h>
#include "main.h"

   static Window *s_main_window;
   static TextLayer *s_time_layer;
   static TextLayer *s_date_layer;
   static TextLayer *s_battery_layer;
   static GBitmap *s_fcn_bitmap;
   static BitmapLayer *s_bitmap_layer;

static void update_time() {
   
   // Get a tm structure
   time_t temp = time(NULL); 
   struct tm *tick_time = localtime(&temp);

   // Create a long-lived buffer
   static char buffer[] = "00:00";
   static char bufferdate[] = "Vendredi 30 Septembre 2000";

   // Write the current hours and minutes into the buffer
   if(clock_is_24h_style() == true) {
     // Use 24 hour format
     strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
   } else {
     // Use 12 hour format
     strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
   }

   strftime(bufferdate, sizeof("Vendredi 30 Septembre 2000"), "%A %d %B %Y", tick_time);   
   
   // Display this time on the TextLayer
   text_layer_set_text(s_time_layer, buffer);
   text_layer_set_text(s_date_layer, bufferdate);
}


static void update_battery(BatteryChargeState charge){
      
   static char s_battery_buffer[32];
   if (charge.is_plugged){
      snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%+", charge.charge_percent);
   } else {
      snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", charge.charge_percent); 
   }
   
   text_layer_set_text(s_battery_layer, s_battery_buffer);
   
}


static void battery_handler(BatteryChargeState charge){

   update_battery(charge);   
   
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {

   update_time();
   
}


static void main_window_load(Window *window) {
   
   Layer *window_layer = window_get_root_layer(window);
   GRect window_bounds = layer_get_bounds(window_layer);
   
   // Create the Bitmap
   s_fcn_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FCN_BW_50);
   s_bitmap_layer = bitmap_layer_create(GRect(0, 0, 50, 50));
   bitmap_layer_set_bitmap(s_bitmap_layer, s_fcn_bitmap);
   
   // Create time TextLayer
   s_time_layer = text_layer_create(GRect(0, 55, window_bounds.size.w, 50));
   text_layer_set_background_color(s_time_layer, GColorBlack);
   text_layer_set_text_color(s_time_layer, GColorClear);
   text_layer_set_text(s_time_layer, "00:00");
   
   // Create battery TextLayer
   s_battery_layer = text_layer_create(GRect(0, 0, window_bounds.size.w, 50));
   text_layer_set_background_color(s_battery_layer, GColorBlack);
   text_layer_set_text_color(s_battery_layer, GColorClear);
   text_layer_set_text(s_battery_layer, "100%");
   
   // Create date TextLayer
   s_date_layer = text_layer_create(GRect(0, 110, window_bounds.size.w, 50));
   text_layer_set_background_color(s_date_layer, GColorBlack);
   text_layer_set_text_color(s_date_layer, GColorClear);
   text_layer_set_text(s_date_layer, "Vendredi 30 Septembre 2000");
   
   
   // Improve the layout to be more like a watchface
   text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
   text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
   text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
   text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
   text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
   text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);

   // Add it as a child layer to the Window's root layer
   layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
   layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
   layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
   layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bitmap_layer));

}

static void main_window_unload(Window *window) {
   
   text_layer_destroy(s_time_layer);
   text_layer_destroy(s_battery_layer);
   text_layer_destroy(s_date_layer);
   gbitmap_destroy(s_fcn_bitmap);
   bitmap_layer_destroy(s_bitmap_layer);

   
}   
   
static void init() {
   
   // Force the app's locale to French
   setlocale(LC_TIME, "fr_FR");
   
   // Register with TickTimerService
   tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
   
   // Register with the BatteryStateService
   battery_state_service_subscribe(battery_handler);
   
   // Create main Window element and assign to pointer
   s_main_window = window_create();
   window_set_background_color(s_main_window,GColorBlack);

   // Set handlers to manage the elements inside the Window
   window_set_window_handlers(s_main_window, (WindowHandlers) {
     .load = main_window_load,
     .unload = main_window_unload
   });

   // Show the Window on the watch, with animated=true
   window_stack_push(s_main_window, true);
   
   update_time();
   update_battery(battery_state_service_peek());
   
}

static void deinit() {
   
   // Destroy Window
   window_destroy(s_main_window);
   
}

int main(void) {
   
   init();
   app_event_loop();
   deinit();
   
}