enum RecurseApi {
    struct Person: Codable {
        let id: Int
        let name: String
    }

    class HubVisit: Codable {
        let person: Person
        let date: String
        let start_date: String?
        let end_date: String?
        // "app_data": {},
        let notes: String
        let created_at: String // "2023-06-03T15:02:50-04:00"
        let updated_at: String // "2023-06-03T15:02:50-04:00"
        let created_by_app: String
        let updated_by_app: String
    }

    class HubVisitUpdate: Codable {
        // "app_data": {}?,
        let notes: String?

        init(notes: String?) {
            self.notes = notes
        }
    }
}
