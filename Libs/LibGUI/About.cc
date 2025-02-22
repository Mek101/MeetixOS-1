/**
 * @brief
 * This file is part of the MeetiX Operating System.
 * Copyright (c) 2017-2022, Marco Cicognani (marco.cicognani@meetixos.org)
 *
 * @developers
 * Marco Cicognani (marco.cicognani@meetixos.org)
 *
 * @license
 * GNU General Public License version 3
 */

#include <LibGraphics/Metrics/Dimension.hh>
#include <LibGUI/About.hh>
#include <LibGUI/Application.hh>
#include <LibGUI/Component/Geoshape.hh>
#include <LibGUI/Component/Label.hh>
#include <LibGUI/Component/Window.hh>
#include <LibUtils/Environment.hh>

// interface object for about
static Window*   about;
static Geoshape* panel;
static Label*    line1;
static Label*    line2;

/*
 *	delete all object
 */
static void close() {
    delete line1;
    delete line2;
    delete panel;
    delete about;
}

/*
 *	show window with version, logo and copyrigth of MeetiX OS
 */
void about_os() {
    // get resolution
    auto rs      = UI::screen_dimension();
    auto wBounds = Graphics::Metrics::Rectangle(rs.width() / 2 - 175, rs.height() / 2 - 115, 350, 230);

    // create and setup window
    about = Window::create();
    about->set_bounds(wBounds);
    about->set_title_alignment(Graphics::Text::Alignment::CENTER);
    about->set_color(Graphics::Color::as_rgb(255, 255, 255), Graphics::Color::as_rgb(0, 0, 0));
    about->set_title("About MeetiX OS");
    about->set_resizable(false);

    // create and setup geoshape as panel of window
    panel = Geoshape::create();
    about->add_child(panel);
    panel->set_bounds(Graphics::Metrics::Rectangle(0, 0, 350, 230));
    panel->set_image("/Apps/OsmosUI/Resources/Icons/OSLogo_hdpi.png", Graphics::Metrics::Point(70, 50));

    // create and configure first line of about text
    line1 = Label::create();
    panel->add_child(line1);
    line1->set_bounds(Graphics::Metrics::Rectangle(0, 5, 320, 30));
    line1->set_title_alignment(Graphics::Text::Alignment::CENTER);
    line1->set_font_size(25);
    line1->set_color(0, Graphics::Color::as_rgb(0, 200, 0));
    line1->set_title("MeetiX OS " + Utils::Environment::version());

    // create and configure second line of about text
    line2 = Label::create();
    panel->add_child(line2);
    line2->set_bounds(Graphics::Metrics::Rectangle(0, 130, 320, 30));
    line2->set_title_alignment(Graphics::Text::Alignment::CENTER);
    line2->set_title("[ Copyright (C) 2021, MeetiX OS Project ]");

    // show window about
    about->set_visible(true);

    // if window close delete al components
    about->on_close(close);
}
