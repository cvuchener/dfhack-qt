Qt plugins for DFHack
=====================

DFHack plugins for using Qt widgets. The qapplication plugin is the central plugin managing Qt Application. Other plugins may create Qt widgets by using qapplication when it is enabled.

How to use
----------

The qapplication plugin must be enabled for any Qt widget to be created. Type
in DFHack console or add in DFHack init file `enable qapplication`.

After that you should be able to run commands creating Qt widgets from this plugin or others.

### Widgets commands

These commands will create Qt windows.

 - `qt-info`: show a simple dialog with version information.
 - `qt-log`: show Qt log messages.

### Configuration commands

These commands must be run before enabling qapplication.

 - `qt-set-style <style-name>`: select the style to be used.
 - `qt-set-icon-theme <theme-name>`: select the icon theme to be used.
 - `qt-register-resource <filename> [mapped-root]`: load and register a binary
   resource file (.rcc).


How to build
------------

First, you must have downloaded [DFHack source code](https://github.com/DFHack/dfhack/) and be able to build it. See
[DFHack documentation](https://dfhack.readthedocs.io/en/stable/docs/Compile.html) for
more details.

Copy/link/clone this directory as a subdirectory in dfhack's `plugins`
directory (e.g. in a `qt` subdirectory). Add the subdirectory (e.g. a
`add_subdirectory(qt)` line) in `plugins/CMakeLists.custom.txt`. The plugin
will be built along other plugins, or you can request to build specifically the
`qapplication` target.


How to create a DFHack plugin using qapplication
------------------------------------------------

Your plugin subdirectory must be added after dfhack-qt's one. In addition to
the required Qt targets, you need to add `dfhack-qt-common` to the link
libraries of your plugin. For example:

```
DFHACK_PLUGIN(myqtplugin
    ${MYQTPLUGIN_SOURCES}
    LINK_LIBRARIES Qt5::Core Qt5::Widget dfhack-qt-common)
```

This library will let you use the helper functions from dfhack-qt include directory.

### Widget creation

For creating Qt widgets from your plugin, you must include `TopLevelWidget.h`,
add call `addTopLevelWidget`, it takes a widget factory and optionally an
output stream for printing errors to the console. The `SimpleWidgetFactory`
template can be used for widgets that can be default constructed.

```
static command_result show_simple_widget(color_ostream &out, std::vector<std::string> &parameters)
{
    return addTopLevelWidget(SimpleWidgetFactory<MySimpleWidget>(), &out) ? CR_OK : CR_FAILURE;
}

static command_result show_complex_widget(color_ostream &out, std::vector<std::string> &parameters)
{
    /* Check and prepare parameters here */

    return addTopLevelWidget([=](){
        auto widget = new MyComplexWidget(some_parameter);

        /* configure widget here */

        return widget;
    }, &out) ? CR_OK : CR_FAILURE;
}
```

`addTopLevelWidget` will return as soon as the widget is queued for creation,
and the factory may not have run yet. Make sure any data captured by the
factory live long enough. It returns a boolean telling if the widget creation
was successfully queued, it does not depend on the factory success.

Once created the application set the "DeleteOnClose" attribute on the new
widget, so it is automatically deleted when it is closed.

Qt event loop and the widget factory run on a different thread from DF and
DFHack plugins. It is recommended to lock a DFHack `CoreSuspender` when
interacting with DF data from Qt thread.

### Utility functions

`utils.h` contains function for helping converting strings between DF std::string and Qt QString (`DF2QT` and `QT2DF`).


About macOS
-----------

Because macOS does not allow interacting with GUI from outside the main thread,
**the qapplication plugin cannot work correctly on macOS**.
