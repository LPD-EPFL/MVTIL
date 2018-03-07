TOPTARGETS := all clean

SUBDIRS := system/tx_2PL system/tx_MVTIL system/tx_MVTO+

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)