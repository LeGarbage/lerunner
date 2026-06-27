{
  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";

  outputs =
    { self, nixpkgs }:
    let
      inherit (nixpkgs) lib;

      pkgsFor = system: nixpkgs.legacyPackages.${system} or (import nixpkgs { inherit system; });
      supportedSystems = lib.systems.doubles.linux;
      forAllSystems = function: lib.genAttrs supportedSystems (system: function (pkgsFor system));
    in
    {
      packages = forAllSystems (pkgs: rec {
        lerunner = pkgs.callPackage (
          {
            stdenv,
            cmake,
            pkg-config,
            gtkmm4,
            gtk4-layer-shell,
            xdg-utils-cxx,
          }:
          stdenv.mkDerivation {
            pname = "lerunner";
            version = "0.0.1";

            src = ./.;

            nativeBuildInputs = [
              cmake
              pkg-config
              xdg-utils-cxx
            ];

            buildInputs = [
              gtkmm4
              gtk4-layer-shell
            ];
          }
        ) { };

        default = lerunner;
      });

      devShells = forAllSystems (pkgs: {
        default =
          with pkgs;
          mkShell {
            inputsFrom = [ self.packages.${stdenv.hostPlatform.system}.lerunner ];
            packages = [ clang-tools ];
          };
      });
    };

}
