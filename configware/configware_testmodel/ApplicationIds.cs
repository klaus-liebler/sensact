using Klli.Sensact.Model.Common;
namespace Klli.Sensact.Model
{

    public enum ApplicationId
    {
        MASTER,
        //Sensact-Geräte
        SNSCT_NODE_HS,
        SNSCT_NODE_UP,
        SNSCT_NODE_SIDEDOOR,
        SNSCT_NODE_TERRASSE,
        PUSHB_X1_XX1_3,
        PUSHB_X2_XX2_4,
        PUSHB_X3_XX3_5,
        PUSHB_X1_XX4__6,
        BLINDB_X2_XX1_7,
        DIM2B_X3_XX2_8,
        POWIT_X1_XX3_9,
        POWIT_X2_XX4_10,
        POWIT_X3_XX1_11,
        POWIT_X1_XX2_12,
        BLIND_X2_XX3_13,
        PWM___X3_XX4_14,
        STDBY_X1_XX1_15,
        SOUND_X2_XX2_0,
        ROTAR_X3_XX3_0,

        FINGR_X1_XX1_42,//Fingerprint
        POWIT_X1_XX1_42,//Türöffner
        REMOT_X1_XX1_42,//Der spezielle Typ Fernbedienung mit Hardcodierten Aktivitäten, wenn ein Knopf gedrückt wird
        PWM___X1_XX1_42,//Spots
        BLIND_X1_XX1_42,//Horizontale Markise
        BLIND_X1_XX1_43,//Vertikale Markise
        NO_APPLICATION=Sensact.Model.Common.Model.NO_APPLICATION_ID
    }
}
