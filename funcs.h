#ifndef FUNCS_H
#define FUNCS_H

#include<stdint.h>
////////////////////////////////

void BRK(void);
void ASL_A(void);
void JSR(void);
void BIT_Z(void);
void BIT_A(void);
void JMP(void);
void ADC_IX(void);
void ADC_Z(void);
void ADC(void);
void ADC_A(void);
void ADC_IY(void);
void STY_Z(void);
void STA_Z(void);
void STX_Z(void);
void STY_A(void);
void STA_A(void);
void STX_A(void);
void STA_ZX(void);
void STA_AX(void);
void LDY_Z(void);
void LDA_Z(void);
void LDX_Z(void);
void LDY(void);
void LDA_IX(void);
void LDX(void);
void LDA(void);
void LDY_A(void);
void LDA_A(void);
void LDX_A(void);
void LDA_IY(void);
void LDY_ZX(void);
void LDA_ZX(void);
void LDX_ZY(void);
void LDA_AY(void);
void LDA_AX(void);
void INY(void);
void BNE(void);
void CPX_I(void);
void INC_Z(void);
void INX(void);
void NOP(void);
void INC_A(void);
void BEQ(void);
void INC_ZX(void);
void INC_AX(void);

#endif