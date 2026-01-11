# Lambda Editor Configuration

## Configuration File

The editor saves user preferences in `config.json` located in:
- **Windows**: `%APPDATA%/Lambda/config.json`
- **macOS**: `~/Library/Application Support/Lambda/config.json`
- **Linux**: `~/.config/Lambda/config.json`

## Configuration Structure

```json
{
  "ui": {
    "statusBar": {
      "fontFamily": "Segoe UI",
      "fontSize": 11,
      "visible": true,
      "showFPS": true,
      "showCPU": true,
      "showGPU": true,
      "showRAM": true,
      "showGrid": true,
      "showCamera": true
    },
    "console": {
      "fontFamily": "Consolas",
      "fontSize": 10,
      "maxLines": 10000,
      "timestamps": true,
      "autoScroll": true,
      "showTrace": false,
      "showDebug": false,
      "showInfo": true,
      "showWarn": true,
      "showError": true,
      "showFatal": true
    },
    "viewport": {
      "backgroundColor": "#2d2d2d",
      "gridColor": "#505050",
      "gridSize": 10.0,
      "showGrid": true,
      "showAxis": true,
      "fov": 75,
      "nearClip": 0.1,
      "farClip": 10000.0
    },
    "window": {
      "width": 1920,
      "height": 1080,
      "maximized": false,
      "inspectorVisible": true,
      "consoleVisible": true,
      "consoleSplitRatio": 70
    }
  },
  "render": {
    "vsync": true,
    "msaaSamples": 4,
    "wireframeMode": false,
    "showNormals": false,
    "showBones": false,
    "showHitboxes": false,
    "showAttachments": false
  },
  "anim": {
    "defaultPlaybackSpeed": 1.0,
    "loopAnimations": true,
    "interpolateFrames": true
  },
  "recentFiles": {
    "paths": [],
    "maxRecent": 10
  }
}
```

## Modifying Configuration

### Through UI (Future)
- Edit → Preferences (Ctrl+,)
- Organized tabs for each section
- Real-time preview of changes

### Manual Editing
1. Close the editor
2. Open `config.json` in text editor
3. Modify values
4. Save and reopen editor

## Configuration Options

### Status Bar
- **fontFamily**: Font family name (string)
- **fontSize**: Font size in points (integer, 8-16)
- **visible**: Show/hide status bar (boolean)
- **show***: Toggle individual status bar fields (boolean)

### Console
- **fontFamily**: Monospace font recommended (string)
- **fontSize**: Font size in points (integer, 8-14)
- **maxLines**: Maximum log lines before truncation (integer, 1000-50000)
- **timestamps**: Show timestamps on each log entry (boolean)
- **autoScroll**: Auto-scroll to bottom on new logs (boolean)
- **show***: Toggle log level visibility (boolean)

### Viewport
- **backgroundColor**: Viewport background (hex color)
- **gridColor**: Grid line color (hex color)
- **gridSize**: Grid spacing in units (float, 1.0-100.0)
- **showGrid**: Display grid (boolean)
- **showAxis**: Display XYZ axis (boolean)
- **fov**: Field of view in degrees (integer, 45-120)
- **nearClip**: Near clipping plane (float, 0.01-10.0)
- **farClip**: Far clipping plane (float, 100.0-100000.0)

### Window
- **width/height**: Window dimensions in pixels (integer)
- **maximized**: Start maximized (boolean)
- **inspectorVisible**: Show inspector panel (boolean)
- **consoleVisible**: Show console panel (boolean)
- **consoleSplitRatio**: Viewport/console split percentage (integer, 10-90)

### Rendering
- **vsync**: Enable vertical sync (boolean)
- **msaaSamples**: Anti-aliasing samples (integer: 0, 2, 4, 8, 16)
- **wireframeMode**: Wireframe rendering (boolean)
- **show***: Overlay display options (boolean)

### Animation
- **defaultPlaybackSpeed**: Default animation speed multiplier (float, 0.1-10.0)
- **loopAnimations**: Loop animations by default (boolean)
- **interpolateFrames**: Smooth frame interpolation (boolean)

### Recent Files
- **paths**: List of recently opened files (array of strings)
- **maxRecent**: Maximum recent files to track (integer, 5-20)

## Reset to Defaults

To reset configuration to defaults:
1. Delete `config.json`
2. Restart the editor
3. Default configuration will be generated

## Implementation Status

- [ ] Config file loading/saving
- [ ] Default value generation
- [ ] UI preferences dialog
- [ ] Per-field validation
- [ ] Hot-reload support
- [ ] Config migration system
