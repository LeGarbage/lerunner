{
  inputs = {
    self.submodules = true;
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

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
            rustPlatform,
            gtk4,
            pkg-config,
          }:
          rustPlatform.buildRustPackage {
            pname = "lerunner";
            version = "0.1.0";

            src = ./.;

            cargoLock.lockFile = ./Cargo.lock;

            nativeBuildInputs = [
              pkg-config
            ];

            buildInputs = [
              gtk4
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
            packages = [
              rust-analyzer
              rustfmt
              clippy
            ];
          };
      });
    };

}
