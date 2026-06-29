#!/usr/bin/env bash
set -euo pipefail

VERSION="${1:-1.0.0}"
PRODUCT="wires-emulator"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$ROOT/build-release"
RELEASE_DIR="$ROOT/releases"
PKG_ROOT="$ROOT/releases/.pkg-root-$VERSION"
ZIP_ROOT="$ROOT/releases/.zip-root-$VERSION"

ARTEFACTS_DIR="$BUILD_DIR/WireEmulator_artefacts/Release"
VST3_SRC="$ARTEFACTS_DIR/VST3/$PRODUCT.vst3"
AU_SRC="$ARTEFACTS_DIR/AU/$PRODUCT.component"

ZIP_NAME="$RELEASE_DIR/${PRODUCT}-${VERSION}-macos-universal.zip"
PKG_NAME="$RELEASE_DIR/${PRODUCT}-${VERSION}-macos-universal.pkg"

echo "==> Building $PRODUCT $VERSION (arm64 + x86_64)"
cmake -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build "$BUILD_DIR" --config Release -j"$(sysctl -n hw.ncpu)"

echo "==> Verifying architectures"
lipo -info "$VST3_SRC/Contents/MacOS/$PRODUCT"
lipo -info "$AU_SRC/Contents/MacOS/$PRODUCT"

echo "==> Creating ZIP"
rm -rf "$ZIP_ROOT"
mkdir -p "$ZIP_ROOT/$PRODUCT/VST3" "$ZIP_ROOT/$PRODUCT/AU"
cp -R "$VST3_SRC" "$ZIP_ROOT/$PRODUCT/VST3/"
cp -R "$AU_SRC" "$ZIP_ROOT/$PRODUCT/AU/"

cat > "$ZIP_ROOT/$PRODUCT/Install.txt" <<EOF
wires-emulator $VERSION — macOS (Apple Silicon + Intel)

Install VST3:
  Copy VST3/wires-emulator.vst3 to ~/Library/Audio/Plug-Ins/VST3/

Install AU:
  Copy AU/wires-emulator.component to ~/Library/Audio/Plug-Ins/Components/

Rescan plugins in your DAW after installation.
EOF

rm -f "$ZIP_NAME"
ditto -c -k --sequesterRsrc --keepParent "$ZIP_ROOT/$PRODUCT" "$ZIP_NAME"

echo "==> Creating PKG"
rm -rf "$PKG_ROOT"
mkdir -p "$PKG_ROOT/Library/Audio/Plug-Ins/VST3"
mkdir -p "$PKG_ROOT/Library/Audio/Plug-Ins/Components"
cp -R "$VST3_SRC" "$PKG_ROOT/Library/Audio/Plug-Ins/VST3/"
cp -R "$AU_SRC" "$PKG_ROOT/Library/Audio/Plug-Ins/Components/"

rm -f "$PKG_NAME"
pkgbuild \
  --root "$PKG_ROOT" \
  --identifier "com.ylab.wires-emulator" \
  --version "$VERSION" \
  --install-location "/" \
  "$PKG_NAME"

rm -rf "$PKG_ROOT" "$ZIP_ROOT"

echo "==> Release artifacts"
ls -lh "$ZIP_NAME" "$PKG_NAME"
