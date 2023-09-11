#include "App.h"

#include <stdexcept>
#include <cstdlib>

#include "../scope_guard/scope_guard.hpp"

#include "../Constants.h"
#include "../utils/Coordinates2d.h"



namespace gui {

	const ALLEGRO_COLOR CELL_COLOR = al_map_rgb(255, 255, 255);
	const ALLEGRO_COLOR WALL_COLOR = al_map_rgb(0, 0, 200);
	const ALLEGRO_COLOR UNDISCOVERED_CELL_COLOR = al_map_rgb(102, 102, 102);
	const ALLEGRO_COLOR UNDISCOVERED_WALL_COLOR = al_map_rgb(51, 51, 51);
	const ALLEGRO_COLOR PATH_COLOR = al_map_rgb(0, 150, 0);

	const ALLEGRO_COLOR START_COLOR = al_map_rgb(255, 153, 204);
	const ALLEGRO_COLOR END_COLOR = al_map_rgb(153, 255, 102);


	const size_t MAZE_CELL_WIDTH = 20;
	const size_t MAZE_WALL_WIDTH = 4;

	const ALLEGRO_COLOR MOUSE_COLOR = al_map_rgb(255, 0, 0);


	App::App(
		const MazeSimulator& maze_simulator,
		MazeDiscovery& maze_discovery,
		Mouse& mouse,
		Discover& discover,
		const std::vector<Algorithm*>& algorithms,
		const MazeCoordinates& start,
		const MazeCoordinates& end
	) :
		maze_simulator(maze_simulator),
		maze_discovery(maze_discovery),
		mouse(mouse),
		discover(discover),
		algorithms(algorithms),
		start(start),
		end(end),
		path_search_state()
	{
	}

	void App::run()
	{
		const auto init = al_init();
		assert(init);

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

		const auto keyboard_install = al_install_keyboard();
		assert(keyboard_install);
		auto keyboard_install_guard = sg::make_scope_guard([&] {
			al_uninstall_keyboard();
			});

		const auto primitives_addon = al_init_primitives_addon();
		assert(primitives_addon);
		auto primitives_addon_guard = sg::make_scope_guard([&] {
			al_shutdown_primitives_addon();
			});

		const auto font_addon = al_init_font_addon();
		assert(font_addon);
		auto font_addon_guard = sg::make_scope_guard([&] {
			al_shutdown_font_addon();
			});

		const auto ttf_addon = al_init_ttf_addon();
		assert(ttf_addon);
		auto ttf_addon_guard = sg::make_scope_guard([&] {
			al_shutdown_ttf_addon();
			});

		al_register_event_source(event_queue, al_get_display_event_source(display));
		// al_register_event_source(event_queue, al_get_timer_event_source(timer));
		al_register_event_source(event_queue, al_get_keyboard_event_source());

		al_clear_to_color(al_map_rgb(0, 0, 0));
		al_flip_display();

		// al_start_timer(timer);

		// initial tick and draw
		draw();

		while (true) {
			ALLEGRO_EVENT event;
			al_wait_for_event(event_queue, &event);

			bool do_tick = false;
			bool do_exit = false;

			switch (event.type) {
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				do_exit = true;
				break;
			case ALLEGRO_EVENT_KEY_CHAR:
				switch (event.keyboard.keycode) {
				case ALLEGRO_KEY_SPACE:
					do_tick = true;
					break;
				case ALLEGRO_KEY_ESCAPE:
					do_exit = true;
					break;
				}
				break;
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
		if (discover.is_completed() && path_search_state.size() > 0) {
			return false;
		}

		if(!discover.is_completed())
			discover.think();

		if (path_search_state.size() == 0 && discover.is_completed())
		{
			create_paths();
		}

		return true;
	}

	void App::draw()
	{
		al_clear_to_color(al_map_rgb(0, 0, 0));

		draw_maze_simulator(simulator_offset);
		draw_box(simulator_offset, start, START_COLOR);
		draw_box(simulator_offset, end, END_COLOR);
		draw_maze_discovery(discovery_offset);
		draw_box(discovery_offset, start, START_COLOR);
		draw_box(discovery_offset, end, END_COLOR);
		draw_discovery_path(discovery_offset);
		draw_mouse(discovery_offset);
		draw_solved_path(simulator_offset, END_COLOR);
		
		al_flip_display();
	}

	void App::draw_box(const WindowCoordinates& offset, const MazeCoordinates& box_coord, const ALLEGRO_COLOR& color) {
		al_draw_filled_rectangle(
			offset.x() + box_coord.x() * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
			offset.y() + box_coord.y() * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
			offset.x() + box_coord.x() * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
			offset.y() + box_coord.y() * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
			color
		);
	}
	
	void App::draw_line(const WindowCoordinates& offset, const MazeCoordinates& start, const MazeCoordinates& end, const ALLEGRO_COLOR& color, float thickness) {
		al_draw_line(
			offset.x() + start.x() * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH / 2,
			offset.y() + start.y() * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH / 2,
			offset.x() + end.x() * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH / 2,
			offset.y() + end.y() * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH / 2,
			color,
			thickness
		);
	}


	void App::draw_maze_simulator(const WindowCoordinates& offset)
	{
		const auto& boxes = maze_simulator.get_boxes();
		const size_t MAZE_SIMULATOR_X_OFFSET = offset.x();
		const size_t MAZE_SIMULATOR_Y_OFFSET = offset.y();


		// inner + left + north walls + background
		for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
			for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
				const auto coord = MazeCoordinates(x, y);
				const auto& box = maze_simulator.get_boxes().get(coord);

				draw_box(offset, coord, CELL_COLOR);

				// draw north wall
				al_draw_filled_rectangle(
					MAZE_SIMULATOR_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					MAZE_SIMULATOR_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
					MAZE_SIMULATOR_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					MAZE_SIMULATOR_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					box.NORTH ? WALL_COLOR : CELL_COLOR
				);

				// draw west wall
				al_draw_filled_rectangle(
					MAZE_SIMULATOR_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
					MAZE_SIMULATOR_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					MAZE_SIMULATOR_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					MAZE_SIMULATOR_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					box.WEST ? WALL_COLOR : CELL_COLOR
				);



				// inner corners
				for (size_t y = 0; y < MAZE_WALL_SIZE - 1; ++y) {
					for (size_t x = 0; x < MAZE_WALL_SIZE - 1; ++x) {
						const auto& us = boxes.get(MazeCoordinates(x, y));
						const auto& next = boxes.get(MazeCoordinates(x + 1, y + 1));

						const bool corner = us.EAST || us.SOUTH || next.NORTH || next.WEST;

						al_draw_filled_rectangle(
							MAZE_SIMULATOR_X_OFFSET + (x + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
							MAZE_SIMULATOR_Y_OFFSET + (y + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
							MAZE_SIMULATOR_X_OFFSET + (x + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
							MAZE_SIMULATOR_Y_OFFSET + (y + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
							corner ? WALL_COLOR : CELL_COLOR
						);
					}
				}

				// right + bottom border
				for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
					const auto& box = boxes.get(MazeCoordinates(MAZE_WALL_SIZE - 1, y));

					// east wall
					al_draw_filled_rectangle(
						MAZE_SIMULATOR_X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						MAZE_SIMULATOR_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						MAZE_SIMULATOR_X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						MAZE_SIMULATOR_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
						box.EAST ? WALL_COLOR : CELL_COLOR
					);
				}
				for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
					const auto& box = boxes.get(MazeCoordinates(x, MAZE_WALL_SIZE - 1));

					al_draw_filled_rectangle(
						MAZE_SIMULATOR_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						MAZE_SIMULATOR_Y_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						MAZE_SIMULATOR_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
						MAZE_SIMULATOR_Y_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						box.SOUTH ? WALL_COLOR : CELL_COLOR
					);
				}

				// outer corners
				for (size_t x = 0; x < MAZE_WALL_SIZE + 1; ++x)
				{
					// north
					al_draw_filled_rectangle(
						MAZE_SIMULATOR_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						MAZE_SIMULATOR_Y_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						MAZE_SIMULATOR_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						MAZE_SIMULATOR_Y_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						WALL_COLOR
					);

					// south
					al_draw_filled_rectangle(
						MAZE_SIMULATOR_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						MAZE_SIMULATOR_Y_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						MAZE_SIMULATOR_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						MAZE_SIMULATOR_Y_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						WALL_COLOR
					);
				}
				for (size_t y = 1; y < MAZE_WALL_SIZE; ++y)
				{
					// west
					al_draw_filled_rectangle(
						MAZE_SIMULATOR_X_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						MAZE_SIMULATOR_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						MAZE_SIMULATOR_X_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						MAZE_SIMULATOR_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						WALL_COLOR
					);

					// east
					al_draw_filled_rectangle(
						MAZE_SIMULATOR_X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						MAZE_SIMULATOR_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
						MAZE_SIMULATOR_X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						MAZE_SIMULATOR_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
						WALL_COLOR
					);
				}
			}
		}

	}

	void App::draw_maze_discovery(const WindowCoordinates& offset)
	{
		const auto& boxes = maze_discovery.get_boxes();
		const size_t MAZE_DISCOVERY_X_OFFSET = offset.x();
		const size_t MAZE_DISCOVERY_Y_OFFSET = offset.y();


		// inner + south + east walls
		for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
			for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
				const auto current = MazeCoordinates(x, y);
				const auto& box = maze_discovery.get_boxes().get(current);

				draw_box(offset, current, (box) ? CELL_COLOR : UNDISCOVERED_CELL_COLOR);

				//east wall
				ALLEGRO_COLOR color_east;

				if (x == MAZE_WALL_SIZE - 1) {
					box.has_value() ? (box.value().EAST ? color_east = WALL_COLOR : color_east = CELL_COLOR) : color_east = UNDISCOVERED_WALL_COLOR;
				}
				else {
					const auto east = MazeCoordinates(x + 1, y);
					const auto& box_east = maze_discovery.get_boxes().get(east);

					if (box.has_value() && box_east.has_value()) {
						box.value().EAST ? color_east = WALL_COLOR : color_east = CELL_COLOR;
					}
					else if (box.has_value() && !box_east.has_value()) {
						box.value().EAST ? color_east = WALL_COLOR : color_east = CELL_COLOR;
					}
					else if (!box.has_value() && box_east.has_value()) {
						box_east.value().WEST ? color_east = WALL_COLOR : color_east = CELL_COLOR;
					}
					else {
						color_east = UNDISCOVERED_WALL_COLOR;
					}
				}

				al_draw_filled_rectangle(
					MAZE_DISCOVERY_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					MAZE_DISCOVERY_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					MAZE_DISCOVERY_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + 2 * MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					MAZE_DISCOVERY_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					color_east
				);

				//south wall
				ALLEGRO_COLOR color_south;
				if (y == MAZE_WALL_SIZE - 1) {
					box.has_value() ? (box.value().SOUTH ? color_south = WALL_COLOR : color_south = CELL_COLOR) : color_south = UNDISCOVERED_WALL_COLOR;
				}
				else {
					const auto south = MazeCoordinates(x, y + 1);
					const auto& box_south = maze_discovery.get_boxes().get(south);

					if (box.has_value() && box_south.has_value()) {
						box.value().SOUTH ? color_south = WALL_COLOR : color_south = CELL_COLOR;
					}
					else if (box.has_value() && !box_south.has_value()) {
						box.value().SOUTH ? color_south = WALL_COLOR : color_south = CELL_COLOR;
					}
					else if (!box.has_value() && box_south.has_value()) {
						box_south.value().NORTH ? color_south = WALL_COLOR : color_south = CELL_COLOR;
					}
					else {
						color_south = UNDISCOVERED_WALL_COLOR;
					}
				}

				// draw south wall
				al_draw_filled_rectangle(
					MAZE_DISCOVERY_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					MAZE_DISCOVERY_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					MAZE_DISCOVERY_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					MAZE_DISCOVERY_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + 2 * MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					color_south
				);

			}
		}


		// outer corners
		for (size_t x = 0; x < MAZE_WALL_SIZE + 1; ++x)
		{
			// north
			al_draw_filled_rectangle(
				MAZE_DISCOVERY_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
				MAZE_DISCOVERY_Y_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
				MAZE_DISCOVERY_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
				MAZE_DISCOVERY_Y_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
				WALL_COLOR
			);

			// south
			al_draw_filled_rectangle(
				MAZE_DISCOVERY_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
				MAZE_DISCOVERY_Y_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
				MAZE_DISCOVERY_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
				MAZE_DISCOVERY_Y_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
				WALL_COLOR
			);
		}
		for (size_t y = 1; y < MAZE_WALL_SIZE; ++y)
		{
			// west
			al_draw_filled_rectangle(
				MAZE_DISCOVERY_X_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
				MAZE_DISCOVERY_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
				MAZE_DISCOVERY_X_OFFSET + 0 * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
				MAZE_DISCOVERY_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
				WALL_COLOR
			);

			// east
			al_draw_filled_rectangle(
				MAZE_DISCOVERY_X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
				MAZE_DISCOVERY_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
				MAZE_DISCOVERY_X_OFFSET + MAZE_WALL_SIZE * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
				MAZE_DISCOVERY_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
				WALL_COLOR
			);
		}

		// north + west border
		for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
			const auto& box = boxes.get(MazeCoordinates(0, y));

			al_draw_filled_rectangle(
				MAZE_DISCOVERY_X_OFFSET,
				MAZE_DISCOVERY_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
				MAZE_DISCOVERY_X_OFFSET + MAZE_WALL_WIDTH,
				MAZE_DISCOVERY_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
				box.has_value() ? (box.value().WEST ? WALL_COLOR : CELL_COLOR) : UNDISCOVERED_WALL_COLOR
			);
		}
		for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
			const auto& box = boxes.get(MazeCoordinates(x, 0));

			al_draw_filled_rectangle(
				MAZE_DISCOVERY_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
				MAZE_DISCOVERY_Y_OFFSET,
				MAZE_DISCOVERY_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
				MAZE_DISCOVERY_Y_OFFSET + MAZE_WALL_WIDTH,
				box.has_value() ? (box.value().NORTH ? WALL_COLOR : CELL_COLOR) : UNDISCOVERED_WALL_COLOR
			);
		}

		// inner corners
		for (size_t y = 0; y < MAZE_WALL_SIZE - 1; ++y) {
			for (size_t x = 0; x < MAZE_WALL_SIZE - 1; ++x) {
				const auto& us = boxes.get(MazeCoordinates(x, y));
				const auto& next = boxes.get(MazeCoordinates(x + 1, y + 1));
				const auto& east = boxes.get(MazeCoordinates(x + 1, y));
				const auto& south = boxes.get(MazeCoordinates(x, y + 1));


				ALLEGRO_COLOR corner_color = CELL_COLOR;
				bool corner = false;
				if (!us.has_value() && !next.has_value() && !east.has_value() && !south.has_value()) {
					corner_color = UNDISCOVERED_WALL_COLOR;
				}
				else if (east.has_value() && (east.value().WEST || east.value().SOUTH)) {
					corner = true;
					corner_color = WALL_COLOR;
				}
				else if (south.has_value() && (south.value().NORTH || south.value().EAST)) {
					corner = true;
					corner_color = WALL_COLOR;
				}
				else if (us.has_value() && (us.value().EAST || us.value().SOUTH)) {
					corner = true;
					corner_color = WALL_COLOR;
				}
				else if (next.has_value() && (next.value().NORTH || next.value().WEST)) {
					corner = true;
					corner_color = WALL_COLOR;
				}
				else {
					corner_color = CELL_COLOR;
				}

				al_draw_filled_rectangle(
					MAZE_DISCOVERY_X_OFFSET + (x + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
					MAZE_DISCOVERY_Y_OFFSET + (y + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH),
					MAZE_DISCOVERY_X_OFFSET + (x + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					MAZE_DISCOVERY_Y_OFFSET + (y + 1) * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					corner_color
				);
			}
		}
	}

	void App::draw_mouse(const WindowCoordinates& offset) {
		const auto& mouse_position = mouse.get_position();

		const size_t MOUSE_X_OFFSET = offset.x();
		const size_t MOUSE_Y_OFFSET = offset.y();

		al_draw_filled_circle(
			MOUSE_X_OFFSET + mouse_position.x() * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH / 2,
			MOUSE_Y_OFFSET + mouse_position.y() * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH / 2,
			MAZE_CELL_WIDTH / 2 - 2,
			MOUSE_COLOR
		);
	}

	/*
	* void App::draw_visited_count() {
		const auto& visited = discover.get_visited();

		for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
			for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
				const auto coord = MazeCoordinates(x, y);
				const auto new_cell_color = al_map_rgb(0, CELL_COLOR.g - visited.count(coord) * 50, 0);

				al_draw_filled_rectangle(
					MAZE_DISCOVERY_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					MAZE_DISCOVERY_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH,
					MAZE_DISCOVERY_X_OFFSET + x * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					MAZE_DISCOVERY_Y_OFFSET + y * (MAZE_CELL_WIDTH + MAZE_WALL_WIDTH) + MAZE_WALL_WIDTH + MAZE_CELL_WIDTH,
					visited.count(coord) > 0 ? new_cell_color : UNDISCOVERED_CELL_COLOR
				);
			}
		}
	}
	*/

	void App::draw_discovery_path(const WindowCoordinates& offset) {
		MazeCoordinates previous = mouse.get_position();

		for (const auto& [position, _] : discover.get_path()) {
			draw_line(offset, position, previous, PATH_COLOR, 2);
			previous = position;
		}
	}

	void App::draw_solved_path(const WindowCoordinates& offset, const ALLEGRO_COLOR& color) {
		float thickness_start = path_search_state.size();
		for (const auto& path : path_search_state) {
			int r = std::rand() % 256;
			int g = std::rand() % 256;
			int b = std::rand() % 256;
			for (auto i = path.value().begin(); i < path.value().end() - 1;) {
				const auto& current =*(i);
				const auto& next = *(++i);
				draw_line(offset, current, next, al_map_rgb(r, g, b), thickness_start*2);
			}
			thickness_start--;
		}
	}

   	void App::create_paths() {
		for (const auto& algorithm : algorithms) {
			if (algorithm->solve(start, end, maze_discovery).has_value()) {
				path_search_state.push_back(algorithm->solve(start, end, maze_discovery));
			}
		}
	}
}