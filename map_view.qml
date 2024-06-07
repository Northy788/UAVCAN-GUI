import QtQuick 2.15
import QtLocation 6.7
import QtPositioning 6.7
import QtQuick.Controls.Fusion

Item {
    id: window
    width:800
    height:600
    Plugin{
        id: mapPlugin
        name: "osm"
        PluginParameter
        {
            name: "osm.mapping.custom.host"
            value: "https://tile.openstreetmap.org/"
        }
    }
    // MainWindow{

    // }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        zoomLevel: 10
        center:  QtPositioning.coordinate(gnssWindow.latitude, gnssWindow.longitude)
        activeMapType: map.supportedMapTypes[map.supportedMapTypes.length - 1]
        // GestureEvent: true
        property geoCoordinate startCentroid
        PinchHandler {
            id: pinch
            target: null
            onActiveChanged: if (active) {
                map.startCentroid = map.toCoordinate(pinch.centroid.position, false)
            }
            onScaleChanged: (delta) => {
                map.zoomLevel += Math.log2(delta)
                map.alignCoordinateToPoint(map.startCentroid, pinch.centroid.position)
            }
            onRotationChanged: (delta) => {
                map.bearing -= delta
                map.alignCoordinateToPoint(map.startCentroid, pinch.centroid.position)
            }
            grabPermissions: PointerHandler.TakeOverForbidden
        }
        WheelHandler {
            id: wheel
            // workaround for QTBUG-87646 / QTBUG-112394 / QTBUG-112432:
            // Magic Mouse pretends to be a trackpad but doesn't work with PinchHandler
            // and we don't yet distinguish mice and trackpads on Wayland either
            acceptedDevices: Qt.platform.pluginName === "cocoa" || Qt.platform.pluginName === "wayland"
                            ? PointerDevice.Mouse | PointerDevice.TouchPad
                            : PointerDevice.Mouse
            rotationScale: 1/120
            property: "zoomLevel"
        }
        DragHandler {
            id: drag
            target: null
            onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
        }
        Shortcut {
            enabled: map.zoomLevel < map.maximumZoomLevel
            sequence: StandardKey.ZoomIn
            onActivated: map.zoomLevel = Math.round(map.zoomLevel + 1)
        }
        Shortcut {
            enabled: map.zoomLevel > map.minimumZoomLevel
            sequence: StandardKey.ZoomOut
            onActivated: map.zoomLevel = Math.round(map.zoomLevel - 1)
        }

        MapQuickItem {
            id: centerMarker
            anchorPoint.x: markerImage.width / 2
            anchorPoint.y: markerImage.height / 2
            coordinate: QtPositioning.coordinate(gnssWindow.latitude, gnssWindow.longitude)

            sourceItem: Image {
                id: markerImage
                source: "qrc:/icons/map-marker-icon-342x512-gd1hf1rz.png"  // Replace with the path to your marker image
                width: 32
                height: 32
            }
        }
    }
    Connections {
            target: gnssWindow
            onLatitudeChanged: map.center.latitude = gnssWindow.latitude
            onLongitudeChanged: map.center.longitude = gnssWindow.longitude
    }
}
