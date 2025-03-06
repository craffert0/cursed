import Foundation
import RealHTTP

class Service {
    let creds: Credentials
    let verbose: Bool

    init(for creds: Credentials,
         verbose: Bool)
    {
        self.creds = creds
        self.verbose = verbose
    }

    private func request(from path: String,
                         method: HTTPMethod = .get,
                         body: Encodable? = nil) async throws -> HTTPResponse
    {
        let req = HTTPRequest {
            $0.url = URL(string: "https://www.recurse.com/" + path)
            $0.method = method
            $0.headers =
                HTTPHeaders(arrayLiteral: .init(name: "Authorization",
                                                value: "Bearer " + creds.token))
            if let body {
                $0.body = .json(body)
            }
        }
        return try await req.fetch()
    }

    func getMe() async throws -> RecurseApi.Person? {
        let response = try await request(from: "api/v1/profiles/me")
        if verbose {
            print(response.statusCode)
            print("DEBUG headers", response.headers)
        }
        guard response.statusCode == .ok else {
            return nil
        }
        return try response.decode(RecurseApi.Person.self)
    }

    func getHubVisits(person id: Int) async throws -> [RecurseApi.HubVisit]? {
        let response =
            try await request(from: "api/v1/hub_visits?person_id=\(id)")
        if verbose {
            print(response.statusCode)
            print("DEBUG headers", response.headers)
        }
        guard response.statusCode == .ok else {
            return nil
        }
        return try response.decode([RecurseApi.HubVisit].self)
    }

    func patchHubVisit(
        from original: RecurseApi.HubVisit,
        to new: RecurseApi.HubVisitUpdate
    ) async throws -> RecurseApi.HubVisit? {
        let path = "api/v1/hub_visits/\(original.person.id)/\(original.date)"
        let response = try await request(from: path,
                                         method: .patch,
                                         body: new)
        if verbose {
            print(response.statusCode)
            print("DEBUG headers", response.headers)
        }
        guard response.statusCode == .ok else {
            return nil
        }
        return try response.decode(RecurseApi.HubVisit.self)
    }

    func deleteHubVisit(from original: RecurseApi.HubVisit) async throws {
        try await deleteHubVisit(for: original.person.id,
                                 on: original.date)
    }

    func deleteHubVisit(for id: Int, on date: String) async throws {
        let path = "api/v1/hub_visits/\(id)/\(date)"
        let response = try await request(from: path,
                                         method: .delete)
        if verbose {
            print(response.statusCode)
            print("DEBUG headers", response.headers)
        }
    }
}
