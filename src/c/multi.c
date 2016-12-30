#include <pebble.h>



static Window *s_window;	
static Layer *marks_layer;
  

struct direction_line_st{
  bool vertical;
  int16_t coord;
};

typedef struct direction_line_st directed_line;

struct place_descrition_st{
  Layer *back_layer;
  Layer *text_layer;
  int32_t offset;
  char place_name[25];
  directed_line line;
};

typedef struct place_descrition_st place_descr;
  
struct cell_position_st{
  int16_t x, y;
  int16_t dx[2];
  int16_t dy[2];
  GPoint min, max;
};

typedef struct cell_position_st cell_position;
  

const int16_t offset_x = 3, offset_y = 3;
const int16_t frame_width = 23;
const int16_t offset_center = 10;




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
  place->offset = offset_t->value->int32;
  strncpy(place->place_name, city_t->value->cstring, sizeof(place->place_name));
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

static void draw_marks(Layer *layer, GContext * ctx){
  GSize size = layer_get_bounds(layer).size;
  GPoint center = GPoint(size.w/2, size.h/2);
  int32_t rad = size.h;
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  for (int32_t ang = 0; ang<TRIG_MAX_ANGLE/2; ang += TRIG_MAX_ANGLE/60){
    GPoint p0 = GPoint((size.w*TRIG_MAX_RATIO/2 + sin_lookup(ang)*rad)/TRIG_MAX_RATIO, 
                       (size.h*TRIG_MAX_RATIO/2 + cos_lookup(ang)*rad)/TRIG_MAX_RATIO);
    GPoint p1 = GPoint((size.w*TRIG_MAX_RATIO/2 + sin_lookup(ang+TRIG_MAX_ANGLE/2)*rad)/TRIG_MAX_RATIO, 
                       (size.h*TRIG_MAX_RATIO/2 + cos_lookup(ang+TRIG_MAX_ANGLE/2)*rad)/TRIG_MAX_RATIO);
    graphics_draw_line(ctx, p0, p1);
  }
  graphics_context_set_stroke_width(ctx, 3);
  for (int32_t ang = 0; ang<TRIG_MAX_ANGLE/2; ang += TRIG_MAX_ANGLE/12){
    GPoint p0 = GPoint((size.w*TRIG_MAX_RATIO/2 + sin_lookup(ang)*rad)/TRIG_MAX_RATIO, 
                       (size.h*TRIG_MAX_RATIO/2 + cos_lookup(ang)*rad)/TRIG_MAX_RATIO);
    GPoint p1 = GPoint((size.w*TRIG_MAX_RATIO/2 + sin_lookup(ang+TRIG_MAX_ANGLE/2)*rad)/TRIG_MAX_RATIO, 
                       (size.h*TRIG_MAX_RATIO/2 + cos_lookup(ang+TRIG_MAX_ANGLE/2)*rad)/TRIG_MAX_RATIO);
    graphics_draw_line(ctx, p0, p1);
  }
  graphics_context_set_antialiased(ctx, false);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, frame_width);
  graphics_draw_rect(ctx, GRect(offset_x+frame_width/2, offset_y+frame_width/2, 
                                size.w-offset_x*2-frame_width, size.h - offset_y*2-1-frame_width));
  
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(offset_x+frame_width+offset_center, offset_y+frame_width+offset_center, 
                                size.w-offset_x*2-frame_width*2-offset_center*2, 
                                size.h - offset_y*2-frame_width*2-1-offset_center*2), 
                    offset_center, GCornersAll);

}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  marks_layer = layer_create(bounds);
  //layer_set_background_color(marks_layer, GColorBlack);
  layer_set_update_proc(marks_layer, draw_marks);
  layer_add_child(window_layer, marks_layer);

}

static void window_unload(Window *window) {
  
}

static void update_time(place_descr *place, time_t *time){
  struct tm *tick_time = gmtime(time);

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