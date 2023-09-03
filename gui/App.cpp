#include "App.h"

#include <stdexcept>
#include "../scope_guard/scope_guard.hpp"

#include "../Constants.h"

namespace gui {

	const ALLEGRO_COLOR CELL_COLOR = al_map_rgb(255, 255, 255);
	const ALLEGRO_COLOR WALL_COLOR = al_map_rgb(0, 0, 200);
	const ALLEGRO_COLOR UNDISCOVERED_CELL_COLOR = al_map_rgb(102, 102, 102);
	const ALLEGRO_COLOR UNDISCOVERED_WALL_COLOR = al_map_rgb(51, 51, 51);

	const size_t MAZE_CELL_SIZE = 10;
	const size_t MAZE_WALL_SIZE = 2;


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
		for (size_t y = 0; y < WALL_SIZE; ++y) {
			for (size_t x = 0; x < WALL_SIZE; ++x) {
				const Coordinates2d<WALL_SIZE, WALL_SIZE>& coord = Coordinates2d<WALL_SIZE, WALL_SIZE>(x, y);
				const auto& box = maze_simulator.get_boxes().get(coord);

				al_draw_filled_rectangle(
					X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
					Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
					X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE + MAZE_CELL_SIZE,
					Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE + MAZE_CELL_SIZE,
					CELL_COLOR
				);

				// draw west wall
				al_draw_filled_rectangle(
					X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
					Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
					X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
					Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE + MAZE_CELL_SIZE,
					box.WEST ? WALL_COLOR : CELL_COLOR
				);

				// draw north wall
				al_draw_filled_rectangle(
					X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
					Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
					X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE + MAZE_CELL_SIZE,
					Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
					box.NORTH ? WALL_COLOR : CELL_COLOR
				);

				// inner corners
				for (size_t y = 0; y < WALL_SIZE - 1; ++y) {
					for (size_t x = 0; x < WALL_SIZE - 1; ++x) {
						const auto& us = boxes.get(MazeSimulator::Coord(x, y));
						const auto& next = boxes.get(MazeSimulator::Coord(x + 1, y + 1));

						const bool corner = us.EAST || us.SOUTH || next.NORTH || next.WEST;

						al_draw_filled_rectangle(
							X_OFFSET + (x + 1) * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
							Y_OFFSET + (y + 1) * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
							X_OFFSET + (x + 1) * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
							Y_OFFSET + (y + 1) * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
							corner ? WALL_COLOR : CELL_COLOR
						);
					}
				}

				// right + bottom border
				for (size_t y = 0; y < WALL_SIZE; ++y) {
					const auto& box = boxes.get(MazeSimulator::Coord(WALL_SIZE - 1, y));

					// east wall
					al_draw_filled_rectangle(
						X_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						X_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE + MAZE_CELL_SIZE,
						box.EAST ? WALL_COLOR : CELL_COLOR
					);
				}
				for (size_t x = 0; x < WALL_SIZE; ++x) {
					const auto& box = boxes.get(MazeSimulator::Coord(x, WALL_SIZE - 1));

					al_draw_filled_rectangle(
						X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						Y_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE + MAZE_CELL_SIZE,
						Y_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						box.SOUTH ? WALL_COLOR : CELL_COLOR
					);
				}

				// outer corners
				for (size_t x = 0; x < WALL_SIZE + 1; ++x)
				{
					// north
					al_draw_filled_rectangle(
						X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						Y_OFFSET + 0 * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						Y_OFFSET + 0 * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						WALL_COLOR
					);

					// south
					al_draw_filled_rectangle(
						X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						Y_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						Y_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						WALL_COLOR
					);
				}
				for (size_t y = 1; y < WALL_SIZE; ++y)
				{
					// west
					al_draw_filled_rectangle(
						X_OFFSET + 0 * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						X_OFFSET + 0 * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						WALL_COLOR
					);

					// east
					al_draw_filled_rectangle(
						X_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						X_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						WALL_COLOR
					);
				}
			}
		}

	}

	void App::draw_maze_discovery()
	{
		const size_t X_OFFSET = 400;
		const size_t Y_OFFSET = 50;

		const auto& boxes = maze_simulator.get_boxes();

		// inner + left + north walls + background
		for (size_t y = 0; y < WALL_SIZE; ++y) {
			for (size_t x = 0; x < WALL_SIZE; ++x) {
				const Coordinates2d<WALL_SIZE, WALL_SIZE>& coord = Coordinates2d<WALL_SIZE, WALL_SIZE>(x, y);
				const auto& box = maze_simulator.get_boxes().get(coord);

				al_draw_filled_rectangle(
					X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
					Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
					X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE + MAZE_CELL_SIZE,
					Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE + MAZE_CELL_SIZE,
					CELL_COLOR
				);

				// draw west wall
				al_draw_filled_rectangle(
					X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
					Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
					X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
					Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE + MAZE_CELL_SIZE,
					box.WEST ? WALL_COLOR : CELL_COLOR
				);

				// draw north wall
				al_draw_filled_rectangle(
					X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
					Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
					X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE + MAZE_CELL_SIZE,
					Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
					box.NORTH ? WALL_COLOR : CELL_COLOR
				);

				// inner corners
				for (size_t y = 0; y < WALL_SIZE - 1; ++y) {
					for (size_t x = 0; x < WALL_SIZE - 1; ++x) {
						const auto& us = boxes.get(MazeSimulator::Coord(x, y));
						const auto& next = boxes.get(MazeSimulator::Coord(x + 1, y + 1));

						const bool corner = us.EAST || us.SOUTH || next.NORTH || next.WEST;

						al_draw_filled_rectangle(
							X_OFFSET + (x + 1) * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
							Y_OFFSET + (y + 1) * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
							X_OFFSET + (x + 1) * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
							Y_OFFSET + (y + 1) * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
							corner ? WALL_COLOR : CELL_COLOR
						);
					}
				}

				// right + bottom border
				for (size_t y = 0; y < WALL_SIZE; ++y) {
					const auto& box = boxes.get(MazeSimulator::Coord(WALL_SIZE - 1, y));

					// east wall
					al_draw_filled_rectangle(
						X_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						X_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE + MAZE_CELL_SIZE,
						box.EAST ? WALL_COLOR : CELL_COLOR
					);
				}
				for (size_t x = 0; x < WALL_SIZE; ++x) {
					const auto& box = boxes.get(MazeSimulator::Coord(x, WALL_SIZE - 1));

					al_draw_filled_rectangle(
						X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						Y_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE + MAZE_CELL_SIZE,
						Y_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						box.SOUTH ? WALL_COLOR : CELL_COLOR
					);
				}

				// outer corners
				for (size_t x = 0; x < WALL_SIZE + 1; ++x)
				{
					// north
					al_draw_filled_rectangle(
						X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						Y_OFFSET + 0 * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						Y_OFFSET + 0 * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						WALL_COLOR
					);

					// south
					al_draw_filled_rectangle(
						X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						Y_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						X_OFFSET + x * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						Y_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						WALL_COLOR
					);
				}
				for (size_t y = 1; y < WALL_SIZE; ++y)
				{
					// west
					al_draw_filled_rectangle(
						X_OFFSET + 0 * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						X_OFFSET + 0 * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						WALL_COLOR
					);

					// east
					al_draw_filled_rectangle(
						X_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE),
						X_OFFSET + WALL_SIZE * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						Y_OFFSET + y * (MAZE_CELL_SIZE + MAZE_WALL_SIZE) + MAZE_WALL_SIZE,
						WALL_COLOR
					);
				}
			}
		}
		/*
		const auto& boxes = maze_discovery.get_boxes();

		size_t current_y = 200;

		for (size_t y = 0; y < WALL_SIZE; ++y) {
			size_t current_x = 200;
			for (size_t x = 0; x < WALL_SIZE; ++x) {
				const auto& coord = Coordinates2d<WALL_SIZE, WALL_SIZE>(x, y);

				const auto& box = boxes.get(coord);
				if (!box) {
					// std::cout << "null" << std::endl;
				}

				current_x += MAZE_CELL_SIZE + MAZE_WALL_SIZE;
			}
			current_y += MAZE_CELL_SIZE + MAZE_WALL_SIZE;
		}
		*/
	}
}