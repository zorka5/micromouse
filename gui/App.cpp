#include "App.h"

#include <stdexcept>
#include "../scope_guard/scope_guard.hpp"

#include "../Constants.h"


namespace gui {

	const ALLEGRO_COLOR CELL_COLOR = al_map_rgb(255, 255, 255);
	const ALLEGRO_COLOR WALL_COLOR = al_map_rgb(0, 0, 200);
	const ALLEGRO_COLOR UNDISCOVERED_CELL_COLOR = al_map_rgb(102, 102, 102);
	const ALLEGRO_COLOR UNDISCOVERED_WALL_COLOR = al_map_rgb(51, 51, 51);

	const size_t MAZE_CELL_WIDTH = 20;
	const size_t MAZE_WALL_WIDTH = 4;


	App::App(const MazeSimulator& maze_simulator, const MazeDiscovery& maze_discovery) :
		maze_simulator(maze_simulator), maze_discovery(maze_discovery)
	{
	}

	void App::run()
	{
		assert(al_init());

		ALLEGRO_TIMER* timer = al_create_timer(1.0 / TICKRATE);
		assert(timer);
		auto timer_guard = sg::make_scope_guard([&] {
			al_destroy_timer(timer);
			});

		ALLEGRO_DISPLAY* display = al_create_display(WINDOW_WIDTH, WINDOW_HEIGHT);
		assert(display);
		auto display_guard = sg::make_scope_guard([&] {
			al_destroy_display(display);
			});

		ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
		assert(event_queue);
		auto event_queue_guard = sg::make_scope_guard([&] {
			al_destroy_event_queue(event_queue);
			});


		assert(al_init_primitives_addon());
		auto primitives_addon_guard = sg::make_scope_guard([&] {
			al_shutdown_primitives_addon();
			});

		al_register_event_source(event_queue, al_get_display_event_source(display));
		al_register_event_source(event_queue, al_get_timer_event_source(timer));

		al_clear_to_color(al_map_rgb(0, 0, 0));
		al_flip_display();

		al_start_timer(timer);

		// initial tick and draw
		tick();
		draw();

		while (true) {
			ALLEGRO_EVENT event;
			al_wait_for_event(event_queue, &event);

			bool do_tick = false;
			bool do_exit = false;

			switch (event.type) {
			case ALLEGRO_EVENT_TIMER: {
				do_tick = true;
				break;
			}
			case ALLEGRO_EVENT_DISPLAY_CLOSE: {
				do_exit = true;
				break;
			}
			default:
				throw std::runtime_error("unknown event");
			}

			if (do_exit)
				break;

			if (do_tick) {
				bool do_draw = tick();
				if (do_draw) {
					draw();
				}
			}
		}
	}

	bool App::tick() {
		return false;
	}

	void App::draw()
	{
		al_clear_to_color(al_map_rgb(0, 0, 0));

		draw_maze_simulator();
		draw_maze_discovery();

		al_flip_display();
	}

	void App::draw_maze_simulator()
	{
		const size_t X_OFFSET = 50;
		const size_t Y_OFFSET = 50;

		const auto& boxes = maze_simulator.get_boxes();

		// inner + left + north walls + background
		for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
			for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
				const auto& coord = MazeCoordinates(x, y);
				const auto& box = maze_simulator.get_boxes().get(coord);

				al_draw_filled_rectangle(
					X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					CELL_COLOR
				);

				// draw west wall
				al_draw_filled_rectangle(
					X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
					Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					box.WEST ? WALL_COLOR : CELL_COLOR
				);

				// draw north wall
				al_draw_filled_rectangle(
					X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
					X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					box.NORTH ? WALL_COLOR : CELL_COLOR
				);

				// inner corners
				for (size_t y = 0; y < MAZE_WALL_SIZE - 1; ++y) {
					for (size_t x = 0; x < MAZE_WALL_SIZE - 1; ++x) {
						const auto& us = boxes.get(MazeCoordinates(x, y));
						const auto& next = boxes.get(MazeCoordinates(x + 1, y + 1));

						const bool corner = us.any();

						al_draw_filled_rectangle(
							X_OFFSET + (x + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
							Y_OFFSET + (y + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
							X_OFFSET + (x + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
							Y_OFFSET + (y + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
							corner ? WALL_COLOR : CELL_COLOR
						);
					}
				}

				// right + bottom border
				for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
					const auto& box = boxes.get(MazeCoordinates(MAZE_WALL_SIZE - 1, y));

					// east wall
					al_draw_filled_rectangle(
						X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
						box.EAST ? WALL_COLOR : CELL_COLOR
					);
				}
				for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
					const auto& box = boxes.get(MazeCoordinates(x, MAZE_WALL_SIZE - 1));

					al_draw_filled_rectangle(
						X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						Y_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
						Y_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						box.SOUTH ? WALL_COLOR : CELL_COLOR
					);
				}

				// outer corners
				for (size_t x = 0; x < MAZE_WALL_SIZE + 1; ++x)
				{
					// north
					al_draw_filled_rectangle(
						X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						Y_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						Y_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						WALL_COLOR
					);

					// south
					al_draw_filled_rectangle(
						X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						Y_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						Y_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						WALL_COLOR
					);
				}
				for (size_t y = 1; y < MAZE_WALL_SIZE; ++y)
				{
					// west
					al_draw_filled_rectangle(
						X_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						X_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						WALL_COLOR
					);

					// east
					al_draw_filled_rectangle(
						X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						WALL_COLOR
					);
				}
			}
		}

	}

	void App::draw_maze_discovery()
	{
		const size_t X_OFFSET = 500;
		const size_t Y_OFFSET = 50;

		const auto& boxes = maze_discovery.get_boxes();

		// inner + left + north walls + background
		for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
			for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
				const MazeCoordinates& coord = MazeCoordinates(x, y);
				const auto& box = maze_discovery.get_boxes().get(coord);

				al_draw_filled_rectangle(
					X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					(box)? CELL_COLOR : UNDISCOVERED_CELL_COLOR
				);
				
				// draw west wall
				al_draw_filled_rectangle(
					X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
					Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					(box.has_value()) ? (box.value().WEST ? WALL_COLOR : CELL_COLOR) : UNDISCOVERED_WALL_COLOR
				);
				
				// draw north wall
				al_draw_filled_rectangle(
					X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
					X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					(box.has_value()) ? (box.value().NORTH ? WALL_COLOR : CELL_COLOR) : UNDISCOVERED_WALL_COLOR
				);
				/*
				// inner corners
				for (size_t y = 0; y < WALL_SIZE - 1; ++y) {
					for (size_t x = 0; x < WALL_SIZE - 1; ++x) {
						const auto& us = boxes.get(MazeCoordinates(x, y));
						const auto& next = boxes.get(MazeCoordinates(x + 1, y + 1));

						const bool corner = us.any();

						al_draw_filled_rectangle(
							X_OFFSET + (x + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
							Y_OFFSET + (y + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
							X_OFFSET + (x + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
							Y_OFFSET + (y + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
							corner ? WALL_COLOR : CELL_COLOR
						);
					}
				}
				*/

				// right + bottom border
				for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
					const auto& box = boxes.get(MazeCoordinates(MAZE_WALL_SIZE - 1, y));

					// east wall
					al_draw_filled_rectangle(
						X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
						(box.has_value()) ? (box.value().EAST ? WALL_COLOR : CELL_COLOR) : UNDISCOVERED_WALL_COLOR
					);
				}
				for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
					const auto& box = boxes.get(MazeCoordinates(x, MAZE_WALL_SIZE - 1));

					al_draw_filled_rectangle(
						X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						Y_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
						Y_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						(box.has_value()) ? (box.value().SOUTH ? WALL_COLOR : CELL_COLOR) : UNDISCOVERED_WALL_COLOR
					);
				}
				/*

				// outer corners
				for (size_t x = 0; x < WALL_SIZE + 1; ++x)
				{
					// north
					al_draw_filled_rectangle(
						X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						Y_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						Y_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						WALL_COLOR
					);

					// south
					al_draw_filled_rectangle(
						X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						Y_OFFSET + WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						Y_OFFSET + WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						WALL_COLOR
					);
				}
				*/
				for (size_t y = 1; y < MAZE_WALL_SIZE; ++y)
				{
					// west
					al_draw_filled_rectangle(
						X_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						X_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						(box) ? WALL_COLOR : UNDISCOVERED_WALL_COLOR
					);

					// east
					al_draw_filled_rectangle(
						X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						(box) ? WALL_COLOR : UNDISCOVERED_WALL_COLOR
					);
				}
				

				
			}
		}
	}
}