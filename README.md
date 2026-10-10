# LeRunner

Like Krunner, but for any Wayland desktop.

## Features

- Custom matching algorithm similar to fzf
- Built-in plugins
  - Application launcher
  - System actions (shut down, restart, etc)
  - Calculator
- Intelligently displays entries based on match scores
  - Not all plugins will have their entries displayed, and more than one can be
    displayed at the same time
  - This behavior is shockingly hard to find on a launcher besides Krunner

## Usage

To use LeRunner, run the `lerunner` binary. This can be done through a terminal,
although for a desktop runner it makes much more sense to launch it through a
keybind in your window manager.

Type the name of the application you would like to launch or system action you
would like to perform. Use the up and down arrow keys or your mouse to select an
entry, and enter or left click to activate it, which will launch the
application. Pressing tab on an entry with an arrow will expand it, presenting
more entries related to the parent, which can each be selected and activated
like any other entry. Pressing escape at any time will close the launcher with
no further action taken.

For the calculator, simply typing in a calculation should cause the launcher to
automatically display the result. To force the use of the calculator, prepend
your calculation with an equals sign (=). Pressing enter on the result of your
calculation will copy it to your clipboard.

## Installation

### The easy way

LeRunner is offered as a nix flake. To install it, add the following input to
your system's `flake.nix`:

```nix
lerunner = {
  url = "git+https://github.com/LeGarbage/lerunner";
  inputs.nixpkgs.follows = "nixpkgs";
};
```

> [!IMPORTANT]
> Due to this project's use of submodules, the more concise `github:` syntax in
> the input cannot be used because of a current limitation of flakes

### The hard way

#### Dependencies

- gtkmm
- gtk4-layer-shell
- libqalculate

#### Build from source

``` sh
# Clone the repo
git clone --recurse-submodules https://github.com/LeGarbage/lerunner.git
cd lerunner

# Build
cmake cmake -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build

# Install (may need root)
cmake --install build
```

## Roadmap

Future plans for LeRunner that will be implemented as I find the motivation to
work on them.

- Faster startup
- Nix runner plugin
- Dynamically loaded, user-created plugins
- Styling
- Configuration
