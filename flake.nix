{
  description = "Android Studio development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      forAllSystems = nixpkgs.lib.genAttrs systems;

    in {
      devShells = forAllSystems (system:
        let
          pkgs = import nixpkgs {
            inherit system;

            config = {
              # Required for Android SDK licenses.
              android_sdk.accept_license = true;
              allowUnfree = true;
            };
          };

          androidSdk = pkgs.androidenv.composeAndroidPackages {
            platformVersions = [ "37" "36" ];
            buildToolsVersions = [ "37.0.0" "36.0.0" ];

            includeEmulator = true;
            includeSystemImages = true;

            systemImageTypes = [ "google_apis" ];
            abiVersions = [ "x86_64" ];

            includeNDK = true;
            ndkVersions = [ "29.0.14033849-rc4" ];

            includeCmake = true;
            cmakeVersions = [ "3.22.1" ];

            includeSources = false;
          };

        in {
          default = pkgs.mkShell {
            packages = with pkgs; [
              android-studio
              androidSdk.androidsdk

              jdk17
              gradle
              kotlin

              # Useful Android command-line tools.
              #adbfs-rootless
            ];

            ANDROID_HOME = "${androidSdk.androidsdk}/libexec/android-sdk";
            ANDROID_SDK_ROOT = "${androidSdk.androidsdk}/libexec/android-sdk";

            JAVA_HOME = "${pkgs.jdk17}";

            GRADLE_USER_HOME = "$HOME/.gradle";

            shellHook = ''
              export PATH="$ANDROID_HOME/platform-tools:$PATH"
              export PATH="$ANDROID_HOME/emulator:$PATH"
              export PATH="$ANDROID_HOME/cmdline-tools/latest/bin:$PATH"
              export PATH="$ANDROID_HOME/tools/bin:$PATH"

              export ANDROID_NDK_ROOT = "$ANDROID_HOME/ndk-bundle";

              # Android Studio and command-line tools expect this directory.
              mkdir -p "$HOME/.android"

              echo "Android development environment loaded"
              echo "ANDROID_HOME=$ANDROID_HOME"
              echo "JAVA_HOME=$JAVA_HOME"
              echo
              echo "Launch Android Studio with:"
              echo "  android-studio"
            '';
          };
        });
    };
}
