# findings-radiolib-code.md — LR2021 limitation/errata/unsupported-feature text from jgromes/RadioLib in-tree sources: docs, changelog, source comments, tests (source-only, verbatim)

## Query log

- [OK] https://api.github.com/repos/jgromes/RadioLib/commits/master -> 200 (1)
- [OK] https://api.github.com/search/code?q=LR2021+repo:jgromes/RadioLib&per_page=100 -> 200 (1)
- [OK] https://api.github.com/repos/jgromes/RadioLib/git/trees/master?recursive=1 -> 200 (1)
- [MISS] https://raw.githubusercontent.com/jgromes/RadioLib/master/CHANGELOG.md -> 404 (3)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/README.md -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/ADSB/ADSB_Monitor/ADSB_Monitor.ino -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/ADSB/ADSB_Receive/ADSB_Receive.ino -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Channel_Activity_Detection_Blocking/LR2021_Channel_Activity_Detection_Blocking.ino -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Channel_Activity_Detection_Interrupt/LR2021_Channel_Activity_Detection_Interrupt.ino -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_GFSK_Modem/LR2021_GFSK_Modem.ino -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_LR_FHSS_Modem/LR2021_LR_FHSS_Modem.ino -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_PingPong/LR2021_PingPong.ino -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Receive_Blocking/LR2021_Receive_Blocking.ino -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Receive_Interrupt/LR2021_Receive_Interrupt.ino -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Receive_MultiSF/LR2021_Receive_MultiSF.ino -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Transmit_Blocking/LR2021_Transmit_Blocking.ino -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Transmit_Interrupt/LR2021_Transmit_Interrupt.ino -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/extras/ADSB_Monitor/ADSBMonitorServer.py -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/extras/test/unit/tests/TestCalculateTimeOnAir.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/extras/test/unit/tests/TestPhyComplete.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/idf_component.yml -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/keywords.txt -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/library.json -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/library.properties -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/RadioLib.h -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/TypeDef.h -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR11x0/LR_common.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR11x0/LR_common.h -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021.h -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_chip_control.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_flrc.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_gfsk.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_lora.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_misc.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_ook.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_oqpsk.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_radio.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_ranging.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_commands.h -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_config.cpp -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_registers.h -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_types.h -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/SX126x/SX126x.h -> 200 (1)
- [OK] https://raw.githubusercontent.com/jgromes/RadioLib/master/src/protocols/LoRaWAN/LoRaWAN.cpp -> 200 (1)

## Artifact blocks

### code-master-head-commit
- location: docs/lr2021-research/errata/raw/radiolib/code-master-head-commit.json
- url: https://api.github.com/repos/jgromes/RadioLib/commits/master
- http_status: 200
- revision_or_publication_date: 2325a38a89dd…
- retrieved_utc: 2026-10-01T05:09:28Z
- bytes: 7793
- sha256: cc4f4997fe56622539e7853576e422500d4ae1c5e91f56627e2f8b93edfac431
- notes: jgromes/RadioLib commits/master HEAD at retrieval
### code-LR2021
- location: docs/lr2021-research/errata/raw/radiolib/code-LR2021.json
- url: https://api.github.com/search/code?q=LR2021+repo:jgromes/RadioLib&per_page=100
- http_status: 200
- revision_or_publication_date: 2325a38a89dd
- retrieved_utc: 2026-10-01T05:09:30Z
- bytes: 188899
- sha256: 0ba4220b30919e2a0bcd28517ef66a8bb2295fff259cee9677549feca525e5cd
- notes: GitHub code-search API response: LR2021 repo:jgromes/RadioLib
### repo-tree-master-recursive.json
- location: docs/lr2021-research/errata/raw/radiolib/repo-tree-master-recursive.json
- url: https://api.github.com/repos/jgromes/RadioLib/git/trees/master?recursive=1
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:35Z
- bytes: 187758
- sha256: bd8804969942b59a16f850246beaa0cc309227626c5af4b8abc6e2dbcea09c92
- notes: recursive tree of jgromes/RadioLib master (409 blobs)
### code-readme.md
- location: docs/lr2021-research/errata/raw/radiolib/code-readme.md
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/README.md
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:45Z
- bytes: 7685
- sha256: bc4fdd4e9609c5a83d3757eba9245aca98aa1b419d92b1c939867b7913dd09e5
- notes: README.md
### code-examples-adsb-adsb_monitor-adsb_monitor.ino
- location: docs/lr2021-research/errata/raw/radiolib/code-examples-adsb-adsb_monitor-adsb_monitor.ino
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/ADSB/ADSB_Monitor/ADSB_Monitor.ino
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:46Z
- bytes: 5002
- sha256: 1c10cd30fc7447f8cae43bb68f3c43d8812752c675f20564bee8a7783fd3fe86
- notes: examples/ADSB/ADSB_Monitor/ADSB_Monitor.ino
### code-examples-adsb-adsb_receive-adsb_receive.ino
- location: docs/lr2021-research/errata/raw/radiolib/code-examples-adsb-adsb_receive-adsb_receive.ino
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/ADSB/ADSB_Receive/ADSB_Receive.ino
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:46Z
- bytes: 6878
- sha256: c386ef22629bd95a49ec2de39c252d9eb35478fe153d1bef10f1caf3f031b5e1
- notes: examples/ADSB/ADSB_Receive/ADSB_Receive.ino
### code-examples-lr2021-lr2021_channel_activity_detection_blocking-lr2021_channel_activity_detection_blocking.ino
- location: docs/lr2021-research/errata/raw/radiolib/code-examples-lr2021-lr2021_channel_activity_detection_blocking-lr2021_channel_activity_detection_blocking.ino
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Channel_Activity_Detection_Blocking/LR2021_Channel_Activity_Detection_Blocking.ino
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:47Z
- bytes: 2407
- sha256: 38091dc7ea492219f908eabcabf55475fb1c2025159acaf976ecaebcd1cab5dd
- notes: examples/LR2021/LR2021_Channel_Activity_Detection_Blocking/LR2021_Channel_Activity_Detection_Blocking.ino
### code-examples-lr2021-lr2021_channel_activity_detection_interrupt-lr2021_channel_activity_detection_interrupt.ino
- location: docs/lr2021-research/errata/raw/radiolib/code-examples-lr2021-lr2021_channel_activity_detection_interrupt-lr2021_channel_activity_detection_interrupt.ino
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Channel_Activity_Detection_Interrupt/LR2021_Channel_Activity_Detection_Interrupt.ino
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:48Z
- bytes: 3329
- sha256: 7ec675280838175be6edd20125806f4540a24a11ded39a971c10accd77bed170
- notes: examples/LR2021/LR2021_Channel_Activity_Detection_Interrupt/LR2021_Channel_Activity_Detection_Interrupt.ino
### code-examples-lr2021-lr2021_gfsk_modem-lr2021_gfsk_modem.ino
- location: docs/lr2021-research/errata/raw/radiolib/code-examples-lr2021-lr2021_gfsk_modem-lr2021_gfsk_modem.ino
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_GFSK_Modem/LR2021_GFSK_Modem.ino
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:49Z
- bytes: 5123
- sha256: 6986da31ea517c6b465d2da30da48edd0c762f6927f1bb784886d78db4282cb0
- notes: examples/LR2021/LR2021_GFSK_Modem/LR2021_GFSK_Modem.ino
### code-examples-lr2021-lr2021_lr_fhss_modem-lr2021_lr_fhss_modem.ino
- location: docs/lr2021-research/errata/raw/radiolib/code-examples-lr2021-lr2021_lr_fhss_modem-lr2021_lr_fhss_modem.ino
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_LR_FHSS_Modem/LR2021_LR_FHSS_Modem.ino
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:51Z
- bytes: 3505
- sha256: 2b2267612fd9e66613b556d4d0cf60250a0302fa5a47184c0b4b9403c588b242
- notes: examples/LR2021/LR2021_LR_FHSS_Modem/LR2021_LR_FHSS_Modem.ino
### code-examples-lr2021-lr2021_pingpong-lr2021_pingpong.ino
- location: docs/lr2021-research/errata/raw/radiolib/code-examples-lr2021-lr2021_pingpong-lr2021_pingpong.ino
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_PingPong/LR2021_PingPong.ino
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:52Z
- bytes: 4429
- sha256: cdefbeae9fe8d61b5b4c80d4c9eadd74f79303a5caf2e8d8566d3f21cae0b4f5
- notes: examples/LR2021/LR2021_PingPong/LR2021_PingPong.ino
### code-examples-lr2021-lr2021_receive_blocking-lr2021_receive_blocking.ino
- location: docs/lr2021-research/errata/raw/radiolib/code-examples-lr2021-lr2021_receive_blocking-lr2021_receive_blocking.ino
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Receive_Blocking/LR2021_Receive_Blocking.ino
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:53Z
- bytes: 3208
- sha256: 7550d11b2559c7476c36cca4677e83bc0d49e710128d06b74aab39b71aabe3f8
- notes: examples/LR2021/LR2021_Receive_Blocking/LR2021_Receive_Blocking.ino
### code-examples-lr2021-lr2021_receive_interrupt-lr2021_receive_interrupt.ino
- location: docs/lr2021-research/errata/raw/radiolib/code-examples-lr2021-lr2021_receive_interrupt-lr2021_receive_interrupt.ino
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Receive_Interrupt/LR2021_Receive_Interrupt.ino
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:54Z
- bytes: 4051
- sha256: 8f03b8c6d081507f1216c0dbeef92f8536818661ed8334219a6ee08f5c08a3e8
- notes: examples/LR2021/LR2021_Receive_Interrupt/LR2021_Receive_Interrupt.ino
### code-examples-lr2021-lr2021_receive_multisf-lr2021_receive_multisf.ino
- location: docs/lr2021-research/errata/raw/radiolib/code-examples-lr2021-lr2021_receive_multisf-lr2021_receive_multisf.ino
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Receive_MultiSF/LR2021_Receive_MultiSF.ino
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:55Z
- bytes: 5714
- sha256: 84adf681e7f134fd9e1e0b04f90e0aab3294ed20d8e24384638329f19dc7b51b
- notes: examples/LR2021/LR2021_Receive_MultiSF/LR2021_Receive_MultiSF.ino
### code-examples-lr2021-lr2021_transmit_blocking-lr2021_transmit_blocking.ino
- location: docs/lr2021-research/errata/raw/radiolib/code-examples-lr2021-lr2021_transmit_blocking-lr2021_transmit_blocking.ino
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Transmit_Blocking/LR2021_Transmit_Blocking.ino
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:56Z
- bytes: 2881
- sha256: f5faeb5f760f932bc5698680a093b5cfa1dad737f356c574a640b3bb4853f3d2
- notes: examples/LR2021/LR2021_Transmit_Blocking/LR2021_Transmit_Blocking.ino
### code-examples-lr2021-lr2021_transmit_interrupt-lr2021_transmit_interrupt.ino
- location: docs/lr2021-research/errata/raw/radiolib/code-examples-lr2021-lr2021_transmit_interrupt-lr2021_transmit_interrupt.ino
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/examples/LR2021/LR2021_Transmit_Interrupt/LR2021_Transmit_Interrupt.ino
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:56Z
- bytes: 3909
- sha256: e6572ddbbf14e1feaf1b140843198e79780ff508fdb717e8337b9b57d14076b5
- notes: examples/LR2021/LR2021_Transmit_Interrupt/LR2021_Transmit_Interrupt.ino
### code-extras-adsb_monitor-adsbmonitorserver.py
- location: docs/lr2021-research/errata/raw/radiolib/code-extras-adsb_monitor-adsbmonitorserver.py
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/extras/ADSB_Monitor/ADSBMonitorServer.py
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:57Z
- bytes: 4736
- sha256: 77cc6b8256751f55095b16bd8c4613440d27b6608bb6d41ac9799d141e7ed73e
- notes: extras/ADSB_Monitor/ADSBMonitorServer.py
### code-extras-test-unit-tests-testcalculatetimeonair.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-extras-test-unit-tests-testcalculatetimeonair.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/extras/test/unit/tests/TestCalculateTimeOnAir.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:58Z
- bytes: 4226
- sha256: 179a3023bf7f6f901bb5039ee1d014760f7a3c1b9b71e5ee59b24700c7e3cb68
- notes: extras/test/unit/tests/TestCalculateTimeOnAir.cpp
### code-extras-test-unit-tests-testphycomplete.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-extras-test-unit-tests-testphycomplete.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/extras/test/unit/tests/TestPhyComplete.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:58Z
- bytes: 6329
- sha256: 0c1ccddddf53063b7c2649f72db2666f266bb467dcaff006fe4f2b5ad1cf7b2c
- notes: extras/test/unit/tests/TestPhyComplete.cpp
### code-idf_component.yml
- location: docs/lr2021-research/errata/raw/radiolib/code-idf_component.yml
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/idf_component.yml
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:09:59Z
- bytes: 855
- sha256: 28813350062ceac489588a5de73b7d78bd4b78c8197b355dfd5babd3daf3111a
- notes: idf_component.yml
### code-keywords.txt
- location: docs/lr2021-research/errata/raw/radiolib/code-keywords.txt
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/keywords.txt
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:00Z
- bytes: 13339
- sha256: b853a067aceede8c91727c0ddbd6b68e2553f43b8c9aefcc4b04ecb9fc30f12f
- notes: keywords.txt
### code-library.json
- location: docs/lr2021-research/errata/raw/radiolib/code-library.json
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/library.json
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:01Z
- bytes: 1033
- sha256: c63a146cbcf8daeeebfa7abf0811b10d8d4931815096941ce733b3ed44a6cde1
- notes: library.json
### code-library.properties
- location: docs/lr2021-research/errata/raw/radiolib/code-library.properties
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/library.properties
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:02Z
- bytes: 491
- sha256: f428c04be0ae83bbb4f3cfdf56610ab833dd6ddcf8de0cf2924e7b2c252e1798
- notes: library.properties
### code-src-radiolib.h
- location: docs/lr2021-research/errata/raw/radiolib/code-src-radiolib.h
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/RadioLib.h
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:03Z
- bytes: 4161
- sha256: 36efebccb1e322df1644873fa3007690248d454c8432f0d096bcfe033d9091d9
- notes: src/RadioLib.h
### code-src-typedef.h
- location: docs/lr2021-research/errata/raw/radiolib/code-src-typedef.h
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/TypeDef.h
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:03Z
- bytes: 18872
- sha256: 6ffa96af9c9d12b95d8681f0f56df5b03cc52babc4bce668010ab599e0afad02
- notes: src/TypeDef.h
### code-src-modules-lr11x0-lr_common.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr11x0-lr_common.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR11x0/LR_common.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:04Z
- bytes: 15306
- sha256: 21d817b68642d8e92e5cf3ebfb40c8b2abe414875edc0b18b8ea40fa93d28f00
- notes: src/modules/LR11x0/LR_common.cpp
### code-src-modules-lr11x0-lr_common.h
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr11x0-lr_common.h
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR11x0/LR_common.h
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:05Z
- bytes: 12874
- sha256: da67a155b770908fe9b1b58b71e0f6025c95cbf281bc12c78307ca5dc7c672ce
- notes: src/modules/LR11x0/LR_common.h
### code-src-modules-lr2021-lr2021.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:06Z
- bytes: 39656
- sha256: 7662c16d8a2e4d10cd1fee7d5910a85a5a246d79f3c76586195af16a2fbbbfa9
- notes: src/modules/LR2021/LR2021.cpp
### code-src-modules-lr2021-lr2021.h
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021.h
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021.h
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:07Z
- bytes: 49778
- sha256: f87cda0d64c9a32f41fbdc963bdc66c3e7fa9fe5c56136f30abc2abc186a4756
- notes: src/modules/LR2021/LR2021.h
### code-src-modules-lr2021-lr2021_cmds_chip_control.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021_cmds_chip_control.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_chip_control.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:08Z
- bytes: 15885
- sha256: f26d5e517e0b411230b2808ca5bd91c8ab0971fdf042e323cee6c0d02b747516
- notes: src/modules/LR2021/LR2021_cmds_chip_control.cpp
### code-src-modules-lr2021-lr2021_cmds_flrc.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021_cmds_flrc.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_flrc.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:09Z
- bytes: 2580
- sha256: 1171c3367d782f8e53179a814d14b698a0704c5a7b49e46c505fa91f6fcd6c6c
- notes: src/modules/LR2021/LR2021_cmds_flrc.cpp
### code-src-modules-lr2021-lr2021_cmds_gfsk.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021_cmds_gfsk.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_gfsk.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:09Z
- bytes: 4542
- sha256: 9adc791f82c937bb662e079706ec97680bb27670617ed841ee5fa33f8758293a
- notes: src/modules/LR2021/LR2021_cmds_gfsk.cpp
### code-src-modules-lr2021-lr2021_cmds_lora.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021_cmds_lora.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_lora.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:10Z
- bytes: 6377
- sha256: dcb35998a2c4be194a70dff0e08f722bcb7c4297d07e1249347d7712e51d7b70
- notes: src/modules/LR2021/LR2021_cmds_lora.cpp
### code-src-modules-lr2021-lr2021_cmds_misc.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021_cmds_misc.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_misc.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:11Z
- bytes: 1208
- sha256: e074ebfa44d130e60a458d47feed202adbcd66aa2ea24b9f3847bbbf5d46e0ba
- notes: src/modules/LR2021/LR2021_cmds_misc.cpp
### code-src-modules-lr2021-lr2021_cmds_ook.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021_cmds_ook.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_ook.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:12Z
- bytes: 4674
- sha256: d6291cf793e9ac109e2f30a9b059ff127cdfa57a25d7dae018d0ca8af4113f00
- notes: src/modules/LR2021/LR2021_cmds_ook.cpp
### code-src-modules-lr2021-lr2021_cmds_oqpsk.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021_cmds_oqpsk.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_oqpsk.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:13Z
- bytes: 2412
- sha256: 3a65cae9ed0e9a525796afeea04047aabe8abe0dd4a7587e72352e690f6651cf
- notes: src/modules/LR2021/LR2021_cmds_oqpsk.cpp
### code-src-modules-lr2021-lr2021_cmds_radio.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021_cmds_radio.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_radio.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:14Z
- bytes: 4811
- sha256: e3a91830e44d33ffb21e7c88e6651709db81842d618153448ab87f7da81571e4
- notes: src/modules/LR2021/LR2021_cmds_radio.cpp
### code-src-modules-lr2021-lr2021_cmds_ranging.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021_cmds_ranging.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_cmds_ranging.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:15Z
- bytes: 2644
- sha256: 9b299e48f8fbdec151f53b0c3096517ce5b09cef2c85420da46ced1b2447e946
- notes: src/modules/LR2021/LR2021_cmds_ranging.cpp
### code-src-modules-lr2021-lr2021_commands.h
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021_commands.h
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_commands.h
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:16Z
- bytes: 48984
- sha256: 39086a393e18eb4e304f5d92031ddea81859ab07715152e2721c9262493981b6
- notes: src/modules/LR2021/LR2021_commands.h
### code-src-modules-lr2021-lr2021_config.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021_config.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_config.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:17Z
- bytes: 45527
- sha256: 93abba9b3ad9cab47c14009534530c6de41506bc483c776a6de4627d557c1fac
- notes: src/modules/LR2021/LR2021_config.cpp
### code-src-modules-lr2021-lr2021_registers.h
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021_registers.h
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_registers.h
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:17Z
- bytes: 1790
- sha256: 9969971a60b7220859bb2533a57224a6ca179f6cb6a2ff4870c22892d62f992a
- notes: src/modules/LR2021/LR2021_registers.h
### code-src-modules-lr2021-lr2021_types.h
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-lr2021-lr2021_types.h
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/LR2021/LR2021_types.h
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:18Z
- bytes: 1154
- sha256: bf24b54f9435f223f517a95af185646bd22c4a54876b5a1c3f18a7f2a523d4bf
- notes: src/modules/LR2021/LR2021_types.h
### code-src-modules-sx126x-sx126x.h
- location: docs/lr2021-research/errata/raw/radiolib/code-src-modules-sx126x-sx126x.h
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/modules/SX126x/SX126x.h
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:20Z
- bytes: 40035
- sha256: 0bf55200bc0ce65681f263c8c8c8f687e34f20a01e13a3e249f1795b5c35de96
- notes: src/modules/SX126x/SX126x.h
### code-src-protocols-lorawan-lorawan.cpp
- location: docs/lr2021-research/errata/raw/radiolib/code-src-protocols-lorawan-lorawan.cpp
- url: https://raw.githubusercontent.com/jgromes/RadioLib/master/src/protocols/LoRaWAN/LoRaWAN.cpp
- http_status: 200
- revision_or_publication_date: 2325a38a89dd0f0e402c19f67fe910111da1e246
- retrieved_utc: 2026-10-01T05:10:21Z
- bytes: 151268
- sha256: bfd64fe3707795fcf54e75fc80e0b21036c0fa5d80850927f5ed7961873c0415
- notes: src/protocols/LoRaWAN/LoRaWAN.cpp

## FAILED / NOT FOUND (radiolib) — code/docs

- query: https://api.github.com/search/code?q=LR2021+repo:jgromes/RadioLib&per_page=100 | hits examined: 41 matching paths from response | final status: OK (total_count=41, items=41)
- query: https://api.github.com/repos/jgromes/RadioLib/git/trees/master?recursive=1 | hits examined: 409 tree blobs enumerated | final status: OK
- query: https://raw.githubusercontent.com/jgromes/RadioLib/master/CHANGELOG.md | hits examined: CHANGELOG.md | final status: MISS (http 404)
