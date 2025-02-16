import Dispatch
import Swifter

var db = Server()
let server = HttpServer()
server["/set"] = { (request: HttpRequest) -> HttpResponse in
    if request.queryParams.count != 1 {
        return .badRequest(.text("you suck"))
    }
    db.set(request.queryParams[0].0, to: request.queryParams[0].1)
    return .created
}

server["/get"] = { (request: HttpRequest) -> HttpResponse in
    if request.queryParams.count != 1 || request.queryParams[0].0 != "key" {
        return .badRequest(.text("you suck"))
    }
    if let value = db.get(name: request.queryParams[0].1) {
        return .ok(.text(value))
    } else {
        return .notFound
    }
}

let semaphore = DispatchSemaphore(value: 0)
do {
    try server.start(4000, forceIPv4: true)
    try print("Server has started ( port = \(server.port()) ). Try to connect now...")
    semaphore.wait()
} catch {
    print("Server start error: \(error)")
    semaphore.signal()
}
