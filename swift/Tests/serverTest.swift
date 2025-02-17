@testable import server
import Testing

@Suite struct ServerTests {
    var server = Server()

    @Test func nothing() {
        #expect(server.get(name: "key") == nil)
    }

    @Test func basics() {
        server.set("key", to: "value")
        #expect(server.get(name: "key") == "value")
    }

    @Test func update() {
        server.set("key", to: "value")
        #expect(server.get(name: "key") == "value")
        server.set("key", to: "value2")
        #expect(server.get(name: "key") == "value2")
    }
}
