// swift-tools-version: 6.0
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "Bad",

    platforms: [.macOS(.v15)],

    products: [
        .library(
            name: "MyLibrary",
            targets: ["MyLibrary"]
        ),
    ],

    targets: [
        .executableTarget(
            name: "Main",
            dependencies: [
                "MyLibrary",
            ]
        ),
        .target(
            name: "MyLibrary"),
    ]
)
