//LGL mod menu

bool KMHack1 = false, KMHack2 = false, KMHack3 = false, HookHack1 = false, HookHack2 = false, HookHack3 = false;
int sliderValue = 1, dmgmul = 1, defmul = 1;
extern "C" {

JNIEXPORT jobjectArray  JNICALL
Java_uk_lgl_modmenu_FloatingModMenuService_getFeatureList(
        JNIEnv *env,
        jobject activityObject) {
    jobjectArray ret;

    const char *features[] = {
		O("1_SeekBar_Damage multiplier_0_500"), 
		O("2_SeekBar_Defense multiplier_0_500")};

    int Total_Feature = (sizeof features /
                         sizeof features[0]); //Now you dont have to manually update the number everytime;

    ret = (jobjectArray) env->NewObjectArray(Total_Feature, env->FindClass(O("java/lang/String")),
                                             env->NewStringUTF(""));
    int i;
    for (i = 0; i < Total_Feature; i++)
	{
		toggles.push_back(false);
		env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));
	}
        
    return (ret);
}

JNIEXPORT void JNICALL
Java_uk_lgl_modmenu_Preferences_Changes(JNIEnv *env, jclass clazz, jobject obj,
                                        jint feature, jint value, jboolean boolean, jstring str) {
    // LOGD("changeToggle");
    //__android_log_print(ANDROID_LOG_VERBOSE, "Mod Menu", "Feature = %d", feature);

    int i = (int) feature;
    switch (i) {

        // Must count from 0
        case 1:
            if (value >= 1) {
                dmgmul = value;
            }
            break;
        case 2:
            if (value >= 1) {
                defmul = value;
            }
			break;
    }
}

}

int (*get_PlayerSide)(void* _this);

int (*orig_CalcAttackDamageWithAdditionalDamageSkill)(void *pThis, int *baseDamage, void *atk, void* def, bool withAttackScene);
int CalcAttackDamageWithAdditionalDamageSkill(void *pThis, int *baseDamage, void *atk, void* def, bool withAttackScene) {
	
	int player = get_PlayerSide(atk);
	LOGD("player %d", player);
	if (player == (1 || 4 || 7 || 9 || 11 || 13 || 16))
	{
		return orig_CalcAttackDamageWithAdditionalDamageSkill(pThis, baseDamage, atk, def, withAttackScene) / defmul;
	}
	else
	{
		return orig_CalcAttackDamageWithAdditionalDamageSkill(pThis, baseDamage, atk, def, withAttackScene) * dmgmul;
	}
	return orig_CalcAttackDamageWithAdditionalDamageSkill(pThis, baseDamage, atk, def, withAttackScene);
}


#define targetLibName O("libil2cpp.so")
// we will run our patches in a new thread so "sleep" doesn't block process main thread
void hack_thread() {

	int i = 0;
    do {
		i++;
		if (i > 15)
		{
			LOGD(O("Detected that the mod is not working. Please restart the game"));
		}
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));
	
	sleep(10);
	
	Il2Cpp::Init(targetLibName);

	#ifdef __arm__
	MSHookFunction((void*)getAbsoluteAddress(libName, 0x0000000)), (void *)CalcAttackDamageWithAdditionalDamageSkill, (void **) &orig_CalcAttackDamageWithAdditionalDamageSkill); //BattleGameMain
	
	get_PlayerSide = (int (*)(void *))getAbsoluteAddress(libName, 0x0000000); //KaijuModel
    #endif

    //return NULL;
}