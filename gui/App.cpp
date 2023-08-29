#include "App.h"
#include "../Constants.h"

#include <iostream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

const float FPS = 1;

App::App(const MazeSimulator& maze_simulator):
	maze_simulator(maze_simulator)
{
}

int App::run()
{

	std::cout << "App run" << std::endl;

	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_EVENT_QUEUE* event_queue = NULL;
	ALLEGRO_TIMER* timer = NULL;

	bool running = true;
	bool redraw = true;

	// Initialize allegro
	if (!al_init()) {
		fprintf(stderr, "Failed to initialize allegro.\n");
		return 1;
	}

	// Initialize the timer
	timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		fprintf(stderr, "Failed to create timer.\n");
		return 1;
	}

	// Create the display
	display = al_create_display(640, 480);
	if (!display) {
		fprintf(stderr, "Failed to create display.\n");
		return 1;
	}

	// Create the event queue
	event_queue = al_create_event_queue();
	if (!event_queue) {
		fprintf(stderr, "Failed to create event queue.");
		return 1;
	}


	al_init_primitives_addon();
	if (!al_init_primitives_addon())
	{
		fprintf(stderr, "Failed to init primitives addon.");
		return -1;
	}
		
	// Register event sources
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	// Display a black screen
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();

	// Start the timer
	al_start_timer(timer);

	// Game loop
	while (running) {
		ALLEGRO_EVENT event;
		ALLEGRO_TIMEOUT timeout;

		// Initialize timeout
		al_init_timeout(&timeout, 0.06);

		// Fetch the event (if one exists)
		bool get_event = al_wait_for_event_until(event_queue, &event, &timeout);

		// Handle the event
		if (get_event) {
			switch (event.type) {
			case ALLEGRO_EVENT_TIMER:
				redraw = true;
				break;
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				running = false;
				break;
			default:
				fprintf(stderr, "Unsupported event received: %d\n", event.type);
				break;
			}
		}

		// Check if we need to redraw
		if (redraw && al_is_event_queue_empty(event_queue)) {
			// Redraw
			al_clear_to_color(al_map_rgb(0, 0, 0));
			draw_maze_similator();
			al_flip_display();
			redraw = false;
		}
	}

	// Clean up
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}

void App::draw_maze_similator()
{
	std::cout << "Draw map" << std::endl;
	const auto & boxes = maze_simulator.get_boxes();

	size_t current_y = 0;
	size_t current_x = 0;
	
	for (size_t y = 0; y < WALL_SIZE; ++y) {
		current_x = 0;
		for (size_t x = 0; x < WALL_SIZE; ++x) {

			const auto& coord = Coordinates2d<WALL_SIZE, WALL_SIZE>(x, y);

			const auto& north = boxes.get(coord).NORTH;
			const auto& south = boxes.get(coord).SOUTH;
			const auto& west = boxes.get(coord).WEST;
			const auto& east = boxes.get(coord).EAST;

			std::cout << north << south << west << east << std::endl;;

			// draw west wall
			if (west == true) {
				al_draw_filled_rectangle(current_x, current_y + MAZE_WALL_SIZE, current_x + MAZE_WALL_SIZE, current_y + MAZE_CELL_SIZE + MAZE_WALL_SIZE, WALL_COLOR);
			}
			else {
				al_draw_filled_rectangle(current_x, current_y + MAZE_WALL_SIZE, current_x + MAZE_WALL_SIZE, current_y + MAZE_CELL_SIZE + MAZE_WALL_SIZE, CELL_COLOR);
			}
			
			// draw north wall
			if (north == true) {
				al_draw_filled_rectangle(current_x + MAZE_WALL_SIZE, current_y, current_x + MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_y + MAZE_WALL_SIZE, WALL_COLOR);
			}
			else {
				al_draw_filled_rectangle(current_x + MAZE_WALL_SIZE, current_y, current_x + MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_y + MAZE_WALL_SIZE, CELL_COLOR);
			}

			//draw eastern wall
			if (east == true) {
				
				al_draw_filled_rectangle(current_x + MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_y + MAZE_WALL_SIZE, current_x + 2*MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_y + MAZE_WALL_SIZE + MAZE_CELL_SIZE, WALL_COLOR);
			}
			else {
				al_draw_filled_rectangle(current_x + MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_y + MAZE_WALL_SIZE, current_x + 2 * MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_y + MAZE_WALL_SIZE + MAZE_CELL_SIZE, CELL_COLOR);
			}

			//draw southern wall
			if (south == true) {
				al_draw_filled_rectangle(current_x + MAZE_WALL_SIZE, current_y + MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_x + MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_y + 2*MAZE_WALL_SIZE + MAZE_CELL_SIZE, WALL_COLOR);
			}
			else {
				al_draw_filled_rectangle(current_x + MAZE_WALL_SIZE, current_y + MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_x + MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_y + 2 * MAZE_WALL_SIZE + MAZE_CELL_SIZE, CELL_COLOR);
			}

			//draw left top corner
			if (north || west) {
				al_draw_filled_rectangle(current_x, current_y, current_x + MAZE_WALL_SIZE, current_y + MAZE_WALL_SIZE, WALL_COLOR);
			}

			//draw right top corner
			if (north || east) {
				al_draw_filled_rectangle(current_x + MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_y, current_x + 2*MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_y + MAZE_WALL_SIZE, WALL_COLOR);
			}

			//draw right bottom corner
			if (east || south) {
				al_draw_filled_rectangle(current_x + MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_y + MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_x + 2 * MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_y + 2*MAZE_WALL_SIZE + MAZE_CELL_SIZE, WALL_COLOR);
			}

			//draw left bottom corner
			if (west || south) {
				al_draw_filled_rectangle(current_x, current_y + MAZE_WALL_SIZE + MAZE_CELL_SIZE, current_x + MAZE_WALL_SIZE, current_y + 2 * MAZE_WALL_SIZE + MAZE_CELL_SIZE, WALL_COLOR);
			}

			
			//draw cell
			al_draw_filled_rectangle(current_x + MAZE_WALL_SIZE, current_y + MAZE_WALL_SIZE, current_x + MAZE_CELL_SIZE + MAZE_WALL_SIZE, current_y + MAZE_CELL_SIZE + MAZE_WALL_SIZE, CELL_COLOR);
			

			current_x += MAZE_CELL_SIZE + MAZE_WALL_SIZE;
		}
		current_y += MAZE_CELL_SIZE + MAZE_WALL_SIZE;
	}

	//al_draw_bitmap_region(maze_simulator_bitmap, sx, sy, sw, sh, dx, dy, 1);
}
