verify:
	cd 1_base64 && $(MAKE) && ./main
	cd 2_fixed_xor && $(MAKE) && ./main
	cd 3_single_byte_xor && $(MAKE) && ./main
	cd 4_detect_single_byte_xor && $(MAKE) && ./main
	cd 5_repeating_key_xor && $(MAKE) && ./main
	cd 6_break_repeated_key_xor && $(MAKE) && ./main
	cd 7_aes_ecb && $(MAKE) && ./main
	cd 8_detect_aes_ecb/ && $(MAKE) && ./main
	cd 9_pkcs7_padding/ && $(MAKE) && ./main
	cd 10_aes_cbc/ && $(MAKE) && ./main
	cd 11_ecb_cbc_detection_oracle/ && $(MAKE) && ./main
