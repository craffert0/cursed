import ArgumentParser
import Foundation

private func json_dump(_ raw: Encodable) throws {
    let encoder = JSONEncoder()
    encoder.outputFormatting = .prettyPrinted
    let data = try encoder.encode(raw)
    print(String(data: data, encoding: .utf8)!)
}

private func today() -> String {
    let date = Date()
    let calendar = Calendar.current
    let year = calendar.component(.year, from: date)
    let month = calendar.component(.month, from: date)
    let day = calendar.component(.day, from: date)
    return String(format: "%04d-%02d-%02d", year, month, day)
}

@main
struct Checkout: AsyncParsableCommand {
    static let configuration = CommandConfiguration(
        abstract: "Checkout from the Hub, and other fun.",
        discussion: """
            ~/.recurserc should contain a login token

                {
                    "token": "6da4231d14..."
                }
            """,
        subcommands: [Checkout.self, Person.self, Visits.self, Update.self,
                      Delete.self],
        defaultSubcommand: Checkout.self
    )

    struct Options: ParsableArguments {
        @Option(name: [.long, .customShort("c")], help: "Credentials file")
        private var creds = "~/.recurserc"

        @Flag(name: [.long, .customShort("v")], help: "Be verbose.")
        var verbose = false

        func service() throws -> Service {
            try Service(for: Credentials(from: creds), verbose: verbose)
        }
    }

    struct Person: AsyncParsableCommand {
        static let configuration =
            CommandConfiguration(abstract: "Print the current user.")

        @OptionGroup var options: Options

        mutating func run() async throws {
            guard let person = try await options.service().getMe() else {
                print("unperson")
                return
            }
            try json_dump(person)
        }
    }

    struct Visits: AsyncParsableCommand {
        static let configuration =
            CommandConfiguration(abstract: "Show the current user's visits.")

        @OptionGroup var options: Options

        mutating func run() async throws {
            let service = try options.service()
            guard let id = try await service.getMe()?.id else {
                print("unperson")
                return
            }
            guard let visits = try await service.getHubVisits(person: id) else {
                print("no visits")
                return
            }
            try json_dump(visits)
        }
    }

    struct Update: AsyncParsableCommand {
        static let configuration =
            CommandConfiguration(abstract: "Update the latest visit.")

        @OptionGroup var options: Options
        @Argument var notes: String

        mutating func run() async throws {
            let service = try options.service()
            guard let id = try await service.getMe()?.id else {
                print("unperson")
                return
            }
            guard let visits = try await service.getHubVisits(person: id), visits.count > 0 else {
                print("no visits")
                return
            }
            let newVisit = RecurseApi.HubVisitUpdate(notes: notes)
            let v2 = try await service.patchHubVisit(from: visits.last!,
                                                     to: newVisit)
            try json_dump(v2!)
        }
    }

    struct Delete: AsyncParsableCommand {
        static let configuration =
            CommandConfiguration(abstract: "Delete the latest visit.")

        @OptionGroup var options: Options

        mutating func run() async throws {
            let service = try options.service()
            guard let id = try await service.getMe()?.id else {
                print("unperson")
                return
            }
            guard let visits = try await service.getHubVisits(person: id), visits.count > 0 else {
                print("no visits")
                return
            }
            try await service.deleteHubVisit(from: visits.last!)
        }
    }

    struct Checkout: AsyncParsableCommand {
        static let configuration =
            CommandConfiguration(abstract: "Delete today's visit.")

        @OptionGroup var options: Options

        mutating func run() async throws {
            let service = try options.service()
            guard let id = try await service.getMe()?.id else {
                print("unperson")
                return
            }
            try await service.deleteHubVisit(for: id, on: today())
            print("checked out")
        }
    }
}
