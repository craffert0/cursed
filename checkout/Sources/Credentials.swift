import Foundation

struct Credentials: Codable {
    let token: String

    init(from filePath: String) throws {
        self = try JSONDecoder().decode(
            Credentials.self,
            from: Data(contentsOf: URL(fileURLWithPath: filePath.replacingOccurrences(of: "~", with: FileManager.default.homeDirectoryForCurrentUser.path)))
        )
    }
}
