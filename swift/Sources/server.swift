class Server {
    private var map_: [String: String] = [:]

    func set(_ name: String, to value: String) {
        map_[name] = value
    }

    func get(name: String) -> String? {
        map_[name]
    }
}
