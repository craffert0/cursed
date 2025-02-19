// swift-tools-version: 6.0
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "server",

    platforms: [.macOS(.v12)],

    dependencies: [
        .package(url: "https://github.com/httpswift/swifter.git", .upToNextMajor(from: "1.5.0")),
    ],

    targets: [
        // Targets are the basic building blocks of a package, defining a module or a test suite.
        // Targets can depend on other targets in this package and products from dependencies.
        .executableTarget(
            name: "server",
            dependencies: [
                .product(name: "Swifter", package: "swifter"),
            ]
        ),
        .testTarget(
            name: "tests",
            dependencies: ["server"]
        ),
    ]
)
