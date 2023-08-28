#include <iostream>

#include "MazeSimulator.h"

#include "Utils.h"

void run() {
	const auto input = read_file_as_string("D:/Documents/Projects/micromouse/data/4.txt");
	const auto maze = MazeSimulator::parse(input);
	std::string ser = maze.serialize();

	std::cout << "FROM FILE: " << std::endl;
	std::cout << input << std::endl;

	std::cout << "FROM SERIALIZING: " << std::endl;
	std::cout << ser << std::endl;
	

	assert(input == ser);
}

int main2()
{
	try {
		run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "non c++ exception handled" << std::endl;
	}
	return 0;

	/*
		MazeSimulator maze_simulator;
		MazeDisocvery maze_discovery;
		Mouse mouse {
			Walls see(direction);
			void move(direction)
		}

		MouseMover mover(maze_simulator, maze_discovery, mouse);
		
		while(true)
		{
			clear_all();

			draw_maze_discovery(maze_simulator);
			draw_maze_discovery(maze_discovery);
			draw_mouse(mouse);

			switch event:
				case SPACE:
					mouse.mover();
		}
	*/
}


#include <allegro5/allegro.h>

const float FPS = 1;

int main() {
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
			al_flip_display();
			redraw = false;
		}
	}

	// Clean up
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}