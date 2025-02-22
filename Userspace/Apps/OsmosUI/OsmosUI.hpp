/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * *
 * MeetiX OS By MeetiX OS Project [Marco Cicognani]                                    *
 *                                                                                     *
 * This program is free software; you can redistribute it and/or                       *
 * modify it under the terms of the GNU General Public License                         *
 * as published by the Free Software Foundation; either version 2                      *
 * of the License, or (char *argumentat your option) any later version.                *
 *                                                                                     *
 * This program is distributed in the hope that it will be useful,                     *
 * but WITHout ANY WARRANTY; without even the implied warranty of                      *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       *
 * GNU General Public License for more details.                                        *
 *                                                                                     *
 * You should have received a copy of the GNU General Public License                   *
 * along with this program; if not, write to the Free Software                         *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * */

#ifndef _OsmosUI_
#define _OsmosUI_

#include <LibGraphics/Metrics/Dimension.hh>
#include <LibGUI/About.hh>
#include <LibGUI/Application.hh>
#include <LibGUI/ButtonList.hh>
#include <LibGUI/Component/Button.hh>
#include <LibGUI/Component/Geoshape.hh>
#include <LibGUI/Component/Label.hh>
#include <LibGUI/Component/TextField.hh>
#include <LibGUI/Component/Window.hh>
#include <LibGUI/Listener/ActionListener.hh>
#include <LibGUI/Listener/KeyListener.hpp>
#include <LibGUI/MessageBox.hh>
#include <map>
#include <math.h>
#include <LibTasking/Lock.hh>

/*
 *
 */
class OsmosUI {
public:
    OsmosUI() {}
    ~OsmosUI();

    // initialize UI
    bool init();

    // configure ui with provided script
    void configureUi(std::string pathToConfiguration, Graphics::Metrics::Dimension resolution);

    // create the components of ui
    void createComponents();

    // main loop of ui
    void mainLoop();

private:
    enum UiStyle_t {
        GNOME,
        KDE,
    };

    // setup MeetiX menu button
    void setMenuButton();

    // setup menu tab
    void setMenuTab();

    // setup memory usage label
    void setMemLabel();

    // setup hour label
    void setHourLabel();

    // set task label
    void setTaskLabel();

    // create app button
    void createMenu();

    // read from keyboard
    IO::Keyboard::Info readInput();

    // resolution of screen
    Graphics::Metrics::Dimension resolution;

    // map that contains configuration provided by .ui.cfg file
    std::map<std::string, std::string> configuration;
    UiStyle_t                          mode;
};

// prototype of method for MeetiX OS Logout
void meetiXOSLogout();

#endif
