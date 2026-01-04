// human_behavior.h
#pragma once
#include <random>
#include <chrono>

class HumanBehaviorSimulator {
private:
    struct BehaviorProfile {
        float reaction_time_mean = 180.0f; // ms
        float reaction_time_std = 25.0f;   // ms
        float accuracy_mean = 0.85f;       // 85%
        float accuracy_std = 0.05f;        // 5%
        float jitter_amount = 0.7f;        // graus
    };
    
    BehaviorProfile profile;
    std::mt19937 rng;
    std::normal_distribution<float> reaction_dist;
    std::normal_distribution<float> accuracy_dist;
    
    // Histórico de ações para análise
    struct ActionRecord {
        std::chrono::steady_clock::time_point timestamp;
        Vector3 target_position;
        float time_taken;
        bool was_kill;
    };
    
    std::vector<ActionRecord> action_history;
    
public:
    HumanBehaviorSimulator() : 
        rng(std::random_device{}()),
        reaction_dist(profile.reaction_time_mean, profile.reaction_time_std),
        accuracy_dist(profile.accuracy_mean, profile.accuracy_std) {}
    
    float GetHumanReactionTime() {
        float time = reaction_dist(rng);
        
        // Limites humanos realistas
        time = std::max(100.0f, std::min(350.0f, time));
        
        // Fadiga ao longo do tempo
        if (action_history.size() > 50) {
            time *= 1.1f; // 10% mais lento
        }
        
        return time;
    }
    
    Vector3 AddHumanError(Vector3 perfect_aim) {
        // Adicionar erro baseado no perfil
        std::uniform_real_distribution<float> angle_dist(0, 2 * M_PI);
        std::normal_distribution<float> distance_dist(0, profile.jitter_amount);
        
        float error_angle = angle_dist(rng);
        float error_distance = distance_dist(rng);
        
        // Converter para offset
        float offset_x = cos(error_angle) * error_distance;
        float offset_y = sin(error_angle) * error_distance;
        
        return Vector3(
            perfect_aim.x + offset_x,
            perfect_aim.y + offset_y,
            perfect_aim.z
        );
    }
    
    void RecordAction(const ActionRecord& record) {
        action_history.push_back(record);
        
        // Manter histórico limitado
        if (action_history.size() > 100) {
            action_history.erase(action_history.begin());
        }
    }
    
    bool ShouldMissShot() {
        // Decidir se deve errar baseado na precisão
        float current_accuracy = accuracy_dist(rng);
        
        // Aumentar chance de errar após muitos acertos
        int recent_hits = CountRecentHits(10);
        if (recent_hits > 8) {
            current_accuracy *= 0.7f;
        }
        
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(rng) > current_accuracy;
    }
};