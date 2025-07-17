#include "Modbus_Master_MFM384.h"


float Random(float n)
{
   return n*rand()/RAND_MAX;
}

float convert_array_uint16_to_float(uint16_t *data, int byte_low, int byte_high)
{
    int byte_low_384 = data[byte_low];
    int byte_high_384 = data[byte_high];
    byte_high_384 <<= 16;
    int value = byte_low_384 + byte_high_384;
    float value_float = *(float *)&value;
    return value_float;
}

char *get_json_update_power_cabinet(parameter_MFM384_t *param_MFM384)
{
    if (param_MFM384 == NULL)
    {
        return NULL;
    }
    char *ret = NULL;

    asprintf(&ret, TEMPLATE_POWER,
             param_MFM384->V1_MFM384,
             param_MFM384->V2_MFM384,
             param_MFM384->V3_MFM384,
             param_MFM384->I1_MFM384,
             param_MFM384->I2_MFM384,
             param_MFM384->I3_MFM384,
             param_MFM384->P1_MFM384,
             param_MFM384->P2_MFM384,
             param_MFM384->P3_MFM384,
             param_MFM384->P_Total_MFM384,
             param_MFM384->PF1_MFM384,
             param_MFM384->PF2_MFM384,
             param_MFM384->PF3_MFM384,
             param_MFM384->F1_MFM384,
             param_MFM384->F2_MFM384,
             param_MFM384->PF3_MFM384
            );
    return ret;
}