################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
inc/%.obj: ../inc/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/opt/ti/ccs9/ccs/tools/compiler/ti-cgt-c2000_18.12.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="D:/workspace/CCS_V9/C17_STEP_MOTOR_OLED_KEY_F2833x_CCSV9_0" --include_path="../inc" --include_path="../DSP2833x_common/include" --include_path="../DSP2833x_headers/include" --include_path="C:/opt/ti/ccs9/ccs/tools/compiler/ti-cgt-c2000_18.12.1.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="inc/$(basename $(<F)).d_raw" --obj_directory="inc" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


