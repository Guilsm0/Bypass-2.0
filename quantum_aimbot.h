// quantum_aimbot.h
#pragma once
#include <cmath>
#include <thread>
#include <atomic>
#include <random>
#include <vector>

class QuantumAimbot {
private:
    struct Target {
        Vector3 position;
        Vector3 velocity;
        float health;
        float distance;
        bool visible;
        uintptr_t entity_ptr;
    };
    
    // Configurações ajustáveis
    struct AimbotConfig {
        float smoothness = 2.5f;       // Suavização do movimento
        float fov = 3.0f;             // Campo de visão em graus
        float prediction = 0.15f;      // Previsão de movimento
        bool visibility_check = true;  // Verificar se visível
        bool human_behavior = true;    // Comportamento humano
        bool randomization = true;     // Aleatorização
    };
    
    AimbotConfig config;
    std::atomic<bool> active{false};
    std::thread aimbot_thread;
    std::mt19937 rng;
    
    // Memória compartilhada com ESP
    struct SharedData {
        std::vector<Target> targets;
        Vector3 local_position;
        Vector3 local_viewangles;
    };
    
    SharedData shared_data;
    
    // Métodos de cálculo
    Vector3 CalculatePrediction(const Target& target) {
        Vector3 predicted = target.position;
        
        // Previsão linear simples
        if (config.prediction > 0.01f) {
            predicted = predicted + (target.velocity * config.prediction);
        }
        
        return predicted;
    }
    
    Vector3 SmoothAim(Vector3 current_angles, Vector3 target_angles) {
        Vector3 delta = target_angles - current_angles;
        
        // Normalizar ângulos
        NormalizeAngles(delta);
        
        // Aplicar suavização
        if (config.smoothness > 1.0f) {
            delta = delta / config.smoothness;
            
            // Adicionar variação humana
            if (config.human_behavior) {
                std::normal_distribution<float> dist(0.0f, 0.3f);
                delta.x += dist(rng);
                delta.y += dist(rng);
            }
        }
        
        return current_angles + delta;
    }
    
    void NormalizeAngles(Vector3& angles) {
        while (angles.x > 180.0f) angles.x -= 360.0f;
        while (angles.x < -180.0f) angles.x += 360.0f;
        while (angles.y > 180.0f) angles.y -= 360.0f;
        while (angles.y < -180.0f) angles.y += 360.0f;
        angles.z = 0.0f;
    }
    
    bool IsInFOV(Vector3 target_angles, Vector3 local_angles) {
        Vector3 delta = target_angles - local_angles;
        NormalizeAngles(delta);
        
        float angle_diff = sqrt(delta.x * delta.x + delta.y * delta.y);
        return angle_diff <= config.fov;
    }
    
    void HumanLikeMovement() {
        // Simular imperfeições humanas
        if (config.human_behavior) {
            // Micro-pausas aleatórias
            std::uniform_int_distribution<int> pause_dist(5, 25);
            std::this_thread::sleep_for(std::chrono::milliseconds(pause_dist(rng)));
            
            // Micro-tremores
            std::normal_distribution<float> shake_dist(0.0f, 0.05f);
            // Aplicar tremores aos ângulos
        }
    }
    
    void AimbotThread() {
        while (active) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            
            if (!shared_data.targets.empty()) {
                // Encontrar melhor alvo
                Target* best_target = nullptr;
                float best_score = std::numeric_limits<float>::max();
                
                for (auto& target : shared_data.targets) {
                    if (!target.visible && config.visibility_check)
                        continue;
                    
                    Vector3 target_angles = CalculateAngleToTarget(target.position);
                    float score = CalculateTargetScore(target, target_angles);
                    
                    if (score < best_score) {
                        best_score = score;
                        best_target = &target;
                    }
                }
                
                // Mirar no alvo
                if (best_target && IsInFOV(
                    CalculateAngleToTarget(best_target->position),
                    shared_data.local_viewangles)) {
                    
                    AimAtTarget(*best_target);
                    HumanLikeMovement();
                }
            }
        }
    }
    
public:
    QuantumAimbot() : rng(std::random_device{}()) {}
    
    void Activate() {
        if (!active) {
            active = true;
            aimbot_thread = std::thread(&QuantumAimbot::AimbotThread, this);
        }
    }
    
    void Deactivate() {
        active = false;
        if (aimbot_thread.joinable())
            aimbot_thread.join();
    }
    
    void UpdateTargets(const std::vector<Target>& new_targets) {
        shared_data.targets = new_targets;
    }
    
    void UpdateLocalData(Vector3 position, Vector3 viewangles) {
        shared_data.local_position = position;
        shared_data.local_viewangles = viewangles;
    }
};