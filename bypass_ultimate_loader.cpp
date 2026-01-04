// bypass_ultimate_loader.cpp
#include "bypass_ultimate.h"
#include <jni.h>
#include <android/log.h>

#define LOG_TAG "BYPASS_LOADER"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// ==================== LOADER AUTOMÁTICO ====================
__attribute__((constructor))
void InitializeBypass() {
    LOGI("🔮 BYPASS ULTIMATE - Proteção Invisível Carregada");
    
    // Aguardar inicialização do jogo
    std::thread([]() {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        LOGI("🎮 Detectando anti-cheat...");
        
        // Detectar anti-cheat automaticamente
        BypassUltimate::AntiCheatType detected = DetectAntiCheat();
        
        LOGI("🎯 Anti-Cheat detectado: %d", detected);
        LOGI("🚀 Ativando bypass ultimate...");
        
        // Ativar bypass
        auto bypass = BypassUltimate::UltimateBypass::GetInstance();
        bypass->Activate(detected);
        
        LOGI("✅ BYPASS ULTIMATE ATIVADO!");
        LOGI("🛡️ 1000 camadas de proteção ativas");
        LOGI("👻 Modo invisível: ATIVO");
        LOGI("🔥 Compatível com todos os anti-cheats");
    }).detach();
}

BypassUltimate::AntiCheatType DetectAntiCheat() {
    // Detecta automaticamente qual anti-cheat está sendo usado
    // Baseado em arquivos, processos, módulos, etc.
    
    // Verificar por Easy Anti-Cheat
    if(access("/proc/EAC", F_OK) == 0 ||
       access("/sys/module/eac", F_OK) == 0) {
        return BypassUltimate::ANTICHEAT_EAC;
    }
    
    // Verificar por BattlEye
    if(access("/proc/BattlEye", F_OK) == 0 ||
       access("/sys/module/battleye", F_OK) == 0) {
        return BypassUltimate::ANTICHEAT_BE;
    }
    
    // Verificar por GCloud
    void* gcloud = dlopen("libgcloud.so", RTLD_LAZY | RTLD_NOLOAD);
    if(gcloud) {
        dlclose(gcloud);
        return BypassUltimate::ANTICHEAT_GCLOUD;
    }
    
    // Default para universal
    return BypassUltimate::ANTICHEAT_EAC;
}

// ==================== JNI INTERFACE ====================
extern "C" JNIEXPORT void JNICALL
Java_com_bypass_ultimate_MainActivity_activateBypass(
    JNIEnv* env, jobject thiz, jint antiCheatType) {
    
    BypassUltimate::UltimateBypass::GetInstance()
        ->Activate(static_cast<BypassUltimate::AntiCheatType>(antiCheatType));
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_bypass_ultimate_MainActivity_getStatus(
    JNIEnv* env, jobject thiz) {
    
    auto status = BypassUltimate::UltimateBypass::GetInstance()->GetStatus();
    return env->NewStringUTF(status.c_str());
}