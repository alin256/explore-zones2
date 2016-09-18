#include <pebble.h>



static Window *s_window;	

  



struct place_descrition{
  Layer *place_layer;
  TextLayer *place_name_layer;
  TextLayer *place_time_layer;
  int32_t offset, x, y;
  char place_name[80];
  char watch_str[8];
  //GColor color;
};

typedef struct place_descrition place_descr;



// Write message to buffer & send
static void send_message(void){
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	
	dict_write_end(iter);
  app_message_outbox_send();
}

static void update_place(place_descr *place, Tuple *city_t, Tuple *offset_t, Tuple* x_t, Tuple* y_t){
  if (!(city_t && offset_t && x_t && y_t))
    return;
  //TODO update only on suibstansial cahnges; make ifs
  place->x = x_t->value->int32;
  place->y = y_t->value->int32;
  place->offset = offset_t->value->int32;
  strncpy(place->place_name, city_t->value->cstring, sizeof(place->place_name));

  snprintf(place->watch_str, sizeof(place->watch_str), "%d", (int)offset_t->value->int32);
  text_layer_set_text(place->place_time_layer, place->watch_str);
  text_layer_set_text(place->place_name_layer, place->place_name);
}





// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}




// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
  Tuple *reason_t = dict_find(received, MESSAGE_KEY_UpdateReason);
	
  if (reason_t){
    uint32_t reason_id = reason_t->value->uint32;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %d", (int) reason_id); 
    {
//       Tuple *city_t;
//       Tuple *offset_t;
//       Tuple *x_t;
//       Tuple *y_t;

//       //updated place 1
//       city_t = dict_find(received, MESSAGE_KEY_Place1);
//       offset_t = dict_find(received, MESSAGE_KEY_ZoneOffset1);
//       x_t = dict_find(received, MESSAGE_KEY_P1X);
//       y_t = dict_find(received, MESSAGE_KEY_P1Y);

    }
  }
  
  //send_message();
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  //TODO make ALL constants variable

}

static void window_unload(Window *window) {
  
}

static void update_time(place_descr *place, time_t *time){
  struct tm *tick_time = gmtime(time);
  strftime(place->watch_str, sizeof(place->watch_str), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Time in %s is updated to %s", place->place_name, place->watch_str); 
  layer_mark_dirty(text_layer_get_layer(place->place_time_layer));
  

}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed){
  time_t now = time(NULL);
}

static void init(void) {
	s_window = window_create();  
  window_set_background_color(s_window, GColorBlack);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(s_window, true);
	
	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);

  // Initialize AppMessage inbox and outbox buffers with a suitable size
  const int inbox_size = 512;
  const int outbox_size = 512;
	app_message_open(inbox_size, outbox_size);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit(void) {
	app_message_deregister_callbacks();
  tick_timer_service_unsubscribe();
	window_destroy(s_window);
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}