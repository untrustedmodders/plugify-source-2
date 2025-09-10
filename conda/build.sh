#!/bin/bash
# build.sh - For Linux builds

set -ex

# Create the target directories
mkdir -p $PREFIX/bin
mkdir -p $PREFIX

# Copy the shared library and plugin file
cp bin/libplugify-plugin-s2sdk.so $PREFIX/bin/
cp gamedata.jsonc $PREFIX/
cp settings.jsonc $PREFIX/
cp plugify-plugin-s2sdk.pplugin $PREFIX/

# Set proper permissions
chmod 755 $PREFIX/bin/libplugify-plugin-s2sdk.so
chmod 644 $PREFIX/gamedata.jsonc
chmod 644 $PREFIX/settings.jsonc
chmod 644 $PREFIX/plugify-plugin-s2sdk.pplugin

# Create activation scripts for proper library path
mkdir -p $PREFIX/etc/conda/activate.d
mkdir -p $PREFIX/etc/conda/deactivate.d

cat > $PREFIX/etc/conda/activate.d/plugify-plugin-s2sdk.sh << EOF
#!/bin/bash
export PLUGIFY_S2SDK_PLUGIN_PATH="\${CONDA_PREFIX}:\${PLUGIFY_S2SDK_PLUGIN_PATH}"
EOF

cat > $PREFIX/etc/conda/deactivate.d/plugify-plugin-s2sdk.sh << EOF
#!/bin/bash
export PLUGIFY_S2SDK_PLUGIN_PATH="\${PLUGIFY_S2SDK_PLUGIN_PATH//\${CONDA_PREFIX}:/}"
EOF

chmod +x $PREFIX/etc/conda/activate.d/plugify-plugin-s2sdk.sh
chmod +x $PREFIX/etc/conda/deactivate.d/plugify-plugin-s2sdk.sh