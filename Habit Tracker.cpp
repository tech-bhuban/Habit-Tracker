

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <iomanip>
#include <algorithm>

struct Habit {
    std::string name;
    std::string category;
    std::string frequency; // "daily", "weekly", "monthly"
    int target;           // Target times per period
    int streak;
    int totalCompleted;
    time_t startDate;
    std::map<time_t, bool> completionLog; // Date -> completed
    
    Habit(const std::string& n, const std::string& cat, 
          const std::string& freq, int targ)
        : name(n), category(cat), frequency(freq), target(targ),
          streak(0), totalCompleted(0), startDate(time(0)) {}
    
    void markComplete(time_t date) {
        completionLog[date] = true;
        totalCompleted++;
        
        // Update streak
        time_t yesterday = date - 86400; // 24 hours in seconds
        if (completionLog.find(yesterday) != completionLog.end() && 
            completionLog[yesterday]) {
            streak++;
        } else {
            streak = 1;
        }
    }
    
    void markIncomplete(time_t date) {
        completionLog[date] = false;
        streak = 0;
    }
    
    double getSuccessRate() const {
        if (completionLog.empty()) return 0;
        
        int completed = 0;
        for (const auto& entry : completionLog) {
            if (entry.second) completed++;
        }
        
        return (static_cast<double>(completed) / completionLog.size()) * 100;
    }
    
    bool isOnTrack() const {
        return streak >= target;
    }
    
    void display() const {
        std::cout << "\n" << std::string(40, '-') << "\n";
        std::cout << "Habit: " << name << " (" << category << ")\n";
        std::cout << "Frequency: " << frequency << " (Target: " << target << ")\n";
        std::cout << "Current Streak: " << streak << " days\n";
        std::cout << "Total Completed: " << totalCompleted << " times\n";
        std::cout << "Success Rate: " << std::fixed << std::setprecision(1) 
                 << getSuccessRate() << "%\n";
        std::cout << "Status: " << (isOnTrack() ? "✅ On Track" : "⚠️ Needs Attention") << "\n";
        std::cout << std::string(40, '-') << "\n";
    }
};

class HabitTracker {
private:
    std::vector<Habit> habits;
    
public:
    void addHabit(const Habit& habit) {
        habits.push_back(habit);
    }
    
    void markHabitComplete(int index) {
        if (index >= 0 && index < habits.size()) {
            habits[index].markComplete(time(0));
            std::cout << "✅ Habit marked as complete!\n";
        }
    }
    
    void displayAllHabits() {
        std::cout << "\n=== HABIT TRACKER ===\n";
        std::cout << "Total Habits: " << habits.size() << "\n\n";
        
        for (size_t i = 0; i < habits.size(); i++) {
            std::cout << i + 1 << ". ";
            habits[i].display();
        }
    }
    
    void displayTodayStatus() {
        time_t today = time(0);
        struct tm* now = localtime(&today);
        
        std::cout << "\n=== TODAY'S HABITS (" 
                 << (now->tm_year + 1900) << "-"
                 << std::setw(2) << std::setfill('0') << (now->tm_mon + 1) << "-"
                 << std::setw(2) << std::setfill('0') << now->tm_mday 
                 << ") ===\n";
        
        int completedToday = 0;
        for (size_t i = 0; i < habits.size(); i++) {
            std::cout << i + 1 << ". " << habits[i].name;
            
            if (habits[i].completionLog.find(today) != habits[i].completionLog.end() &&
                habits[i].completionLog[today]) {
                std::cout << " ✅\n";
                completedToday++;
            } else {
                std::cout << " ❌\n";
            }
        }
        
        std::cout << "\nProgress: " << completedToday << "/" << habits.size() 
                 << " habits completed today\n";
        
        if (habits.size() > 0) {
            double progress = (static_cast<double>(completedToday) / habits.size()) * 100;
            std::cout << "Completion: " << std::fixed << std::setprecision(1) 
                     << progress << "%\n";
            
            // Progress bar
            int barWidth = 50;
            std::cout << "[";
            int pos = barWidth * progress / 100;
            for (int i = 0; i < barWidth; ++i) {
                if (i < pos) std::cout << "=";
                else if (i == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << int(progress) << "%\n";
        }
    }
    
    void displayStatistics() {
        if (habits.empty()) {
            std::cout << "No habits to display statistics.\n";
            return;
        }
        
        std::map<std::string, int> categoryCount;
        int totalStreak = 0;
        double totalSuccessRate = 0;
        Habit* longestStreak = &habits[0];
        Habit* highestSuccess = &habits[0];
        
        for (const auto& habit : habits) {
            categoryCount[habit.category]++;
            totalStreak += habit.streak;
            totalSuccessRate += habit.getSuccessRate();
            
            if (habit.streak > longestStreak->streak) {
                longestStreak = const_cast<Habit*>(&habit);
            }
            if (habit.getSuccessRate() > highestSuccess->getSuccessRate()) {
                highestSuccess = const_cast<Habit*>(&habit);
            }
        }
        
        std::cout << "\n=== HABIT STATISTICS ===\n";
        std::cout << "Total Habits: " << habits.size() << "\n";
        std::cout << "Average Streak: " << totalStreak / habits.size() << " days\n";
        std::cout << "Average Success Rate: " << std::fixed << std::setprecision(1) 
                 << totalSuccessRate / habits.size() << "%\n";
        
        std::cout << "\nBy Category:\n";
        for (const auto& pair : categoryCount) {
            std::cout << "- " << pair.first << ": " << pair.second << " habits\n";
        }
        
        std::cout << "\n🏆 Longest Streak: " << longestStreak->name 
                 << " (" << longestStreak->streak << " days)\n";
        std::cout << "⭐ Highest Success Rate: " << highestSuccess->name 
                 << " (" << std::fixed << std::setprecision(1) 
                 << highestSuccess->getSuccessRate() << "%)\n";
        
        // Success rate distribution
        std::cout << "\nSuccess Rate Distribution:\n";
        std::cout << "90-100%: " << countBySuccessRange(90, 100) << " habits\n";
        std::cout << "70-89%: " << countBySuccessRange(70, 89) << " habits\n";
        std::cout << "50-69%: " << countBySuccessRange(50, 69) << " habits\n";
        std::cout << "Below 50%: " << countBySuccessRange(0, 49) << " habits\n";
    }
    
    int countBySuccessRange(int min, int max) {
        int count = 0;
        for (const auto& habit : habits) {
            double rate = habit.getSuccessRate();
            if (rate >= min && rate <= max) {
                count++;
            }
        }
        return count;
    }
    
    void displayMotivation() {
        if (habits.empty()) return;
        
        std::vector<std::string> motivations = {
            "🌟 Small habits make a big difference!",
            "💪 Consistency is key to success!",
            "🎯 You're closer than you think!",
            "🔥 Keep the streak alive!",
            "🚀 Progress, not perfection!",
            "🌈 Every day is a new opportunity!"
        };
        
        srand(time(0));
        std::cout << "\n💬 Motivation: " 
                 << motivations[rand() % motivations.size()] << "\n";
    }
};

int main() {
    HabitTracker tracker;
    
    // Add sample habits
    tracker.addHabit(Habit("Morning Meditation", "Wellness", "daily", 1));
    tracker.addHabit(Habit("Exercise", "Fitness", "daily", 1));
    tracker.addHabit(Habit("Read 20 Pages", "Learning", "daily", 1));
    tracker.addHabit(Habit("Drink 8 Glasses Water", "Health", "daily", 1));
    tracker.addHabit(Habit("Journal", "Mental Health", "daily", 1));
    
    int choice;
    do {
        std::cout << "\n=== HABIT TRACKER ===\n";
        std::cout << "1. View All Habits\n";
        std::cout << "2. Today's Status\n";
        std::cout << "3. Mark Habit Complete\n";
        std::cout << "4. View Statistics\n";
        std::cout << "5. Get Motivation\n";
        std::cout << "6. Add New Habit\n";
        std::cout << "7. Exit\n";
        std::cout << "Choice: ";
        std::cin >> choice;
        std::cin.ignore();
        
        if (choice == 1) {
            tracker.displayAllHabits();
        } else if (choice == 2) {
            tracker.displayTodayStatus();
        } else if (choice == 3) {
            tracker.displayAllHabits();
            int habitIndex;
            std::cout << "\nEnter habit number to mark complete: ";
            std::cin >> habitIndex;
            tracker.markHabitComplete(habitIndex - 1);
        } else if (choice == 4) {
            tracker.displayStatistics();
        } else if (choice == 5) {
            tracker.displayMotivation();
        } else if (choice == 6) {
            std::string name, category, frequency;
            int target;
            
            std::cout << "Enter habit name: ";
            std::getline(std::cin, name);
            std::cout << "Enter category: ";
            std::getline(std::cin, category);
            std::cout << "Enter frequency (daily/weekly/monthly): ";
            std::getline(std::cin, frequency);
            std::cout << "Enter target times per period: ";
            std::cin >> target;
            
            tracker.addHabit(Habit(name, category, frequency, target));
            std::cout << "Habit added successfully!\n";
        }
    } while (choice != 7);
    
    return 0;
}
