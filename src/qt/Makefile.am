include $(top_srcdir)/src/Makefile.include

AM_CPPFLAGS += -I$(top_srcdir)/src \
  -I$(top_builddir)/src/qt \
  -I$(top_builddir)/src/qt/forms \
  $(PROTOBUF_CFLAGS) \
  $(QR_CFLAGS)
bin_PROGRAMS = ticoin-qt
noinst_LIBRARIES = libticoinqt.a
SUBDIRS = . $(BUILD_TEST_QT)
DIST_SUBDIRS = . test

# ticoin qt core #
QT_TS = \
  locale/ticoin_ach.ts \
  locale/ticoin_af_ZA.ts \
  locale/ticoin_ar.ts \
  locale/ticoin_be_BY.ts \
  locale/ticoin_bg.ts \
  locale/ticoin_bs.ts \
  locale/ticoin_ca_ES.ts \
  locale/ticoin_ca.ts \
  locale/ticoin_ca@valencia.ts \
  locale/ticoin_cmn.ts \
  locale/ticoin_cs.ts \
  locale/ticoin_cy.ts \
  locale/ticoin_da.ts \
  locale/ticoin_de.ts \
  locale/ticoin_el_GR.ts \
  locale/ticoin_en.ts \
  locale/ticoin_eo.ts \
  locale/ticoin_es_CL.ts \
  locale/ticoin_es_DO.ts \
  locale/ticoin_es_MX.ts \
  locale/ticoin_es.ts \
  locale/ticoin_es_UY.ts \
  locale/ticoin_et.ts \
  locale/ticoin_eu_ES.ts \
  locale/ticoin_fa_IR.ts \
  locale/ticoin_fa.ts \
  locale/ticoin_fi.ts \
  locale/ticoin_fr_CA.ts \
  locale/ticoin_fr.ts \
  locale/ticoin_gl.ts \
  locale/ticoin_gu_IN.ts \
  locale/ticoin_he.ts \
  locale/ticoin_hi_IN.ts \
  locale/ticoin_hr.ts \
  locale/ticoin_hu.ts \
  locale/ticoin_id_ID.ts \
  locale/ticoin_it.ts \
  locale/ticoin_ja.ts \
  locale/ticoin_ka.ts \
  locale/ticoin_kk_KZ.ts \
  locale/ticoin_ko_KR.ts \
  locale/ticoin_ky.ts \
  locale/ticoin_la.ts \
  locale/ticoin_lt.ts \
  locale/ticoin_lv_LV.ts \
  locale/ticoin_mn.ts \
  locale/ticoin_ms_MY.ts \
  locale/ticoin_nb.ts \
  locale/ticoin_nl.ts \
  locale/ticoin_pam.ts \
  locale/ticoin_pl.ts \
  locale/ticoin_pt_BR.ts \
  locale/ticoin_pt_PT.ts \
  locale/ticoin_ro_RO.ts \
  locale/ticoin_ru.ts \
  locale/ticoin_sah.ts \
  locale/ticoin_sk.ts \
  locale/ticoin_sl_SI.ts \
  locale/ticoin_sq.ts \
  locale/ticoin_sr.ts \
  locale/ticoin_sv.ts \
  locale/ticoin_th_TH.ts \
  locale/ticoin_tr.ts \
  locale/ticoin_uk.ts \
  locale/ticoin_ur_PK.ts \
  locale/ticoin_uz@Cyrl.ts \
  locale/ticoin_vi.ts \
  locale/ticoin_vi_VN.ts \
  locale/ticoin_zh_CN.ts \
  locale/ticoin_zh_HK.ts \
  locale/ticoin_zh_TW.ts

QT_FORMS_UI = \
  forms/aboutdialog.ui \
  forms/addressbookpage.ui \
  forms/askpassphrasedialog.ui \
  forms/coincontroldialog.ui \
  forms/editaddressdialog.ui \
  forms/helpmessagedialog.ui \
  forms/intro.ui \
  forms/openuridialog.ui \
  forms/optionsdialog.ui \
  forms/overviewpage.ui \
  forms/receivecoinsdialog.ui \
  forms/receiverequestdialog.ui \
  forms/rpcconsole.ui \
  forms/sendcoinsdialog.ui \
  forms/sendcoinsentry.ui \
  forms/signverifymessagedialog.ui \
  forms/transactiondescdialog.ui

QT_MOC_CPP = \
  moc_addressbookpage.cpp \
  moc_addresstablemodel.cpp \
  moc_askpassphrasedialog.cpp \
  moc_ticoinaddressvalidator.cpp \
  moc_ticoinamountfield.cpp \
  moc_ticoingui.cpp \
  moc_ticoinunits.cpp \
  moc_clientmodel.cpp \
  moc_coincontroldialog.cpp \
  moc_coincontroltreewidget.cpp \
  moc_csvmodelwriter.cpp \
  moc_editaddressdialog.cpp \
  moc_guiutil.cpp \
  moc_intro.cpp \
  moc_macdockiconhandler.cpp \
  moc_macnotificationhandler.cpp \
  moc_monitoreddatamapper.cpp \
  moc_notificator.cpp \
  moc_openuridialog.cpp \
  moc_optionsdialog.cpp \
  moc_optionsmodel.cpp \
  moc_overviewpage.cpp \
  moc_paymentserver.cpp \
  moc_qvalidatedlineedit.cpp \
  moc_qvaluecombobox.cpp \
  moc_receivecoinsdialog.cpp \
  moc_receiverequestdialog.cpp \
  moc_recentrequeststablemodel.cpp \
  moc_rpcconsole.cpp \
  moc_sendcoinsdialog.cpp \
  moc_sendcoinsentry.cpp \
  moc_signverifymessagedialog.cpp \
  moc_splashscreen.cpp \
  moc_trafficgraphwidget.cpp \
  moc_transactiondesc.cpp \
  moc_transactiondescdialog.cpp \
  moc_transactionfilterproxy.cpp \
  moc_transactiontablemodel.cpp \
  moc_transactionview.cpp \
  moc_utilitydialog.cpp \
  moc_walletframe.cpp \
  moc_walletmodel.cpp \
  moc_walletview.cpp

ticoin_MM = \
  macdockiconhandler.mm \
  macnotificationhandler.mm

QT_MOC = \
  ticoin.moc \
  intro.moc \
  overviewpage.moc \
  rpcconsole.moc

QT_QRC_CPP = qrc_ticoin.cpp
QT_QRC = ticoin.qrc

PROTOBUF_CC = paymentrequest.pb.cc
PROTOBUF_H = paymentrequest.pb.h
PROTOBUF_PROTO = paymentrequest.proto

ticoin_QT_H = \
  addressbookpage.h \
  addresstablemodel.h \
  askpassphrasedialog.h \
  ticoinaddressvalidator.h \
  ticoinamountfield.h \
  ticoingui.h \
  ticoinunits.h \
  clientmodel.h \
  coincontroldialog.h \
  coincontroltreewidget.h \
  csvmodelwriter.h \
  editaddressdialog.h \
  guiconstants.h \
  guiutil.h \
  intro.h \
  macdockiconhandler.h \
  macnotificationhandler.h \
  monitoreddatamapper.h \
  notificator.h \
  openuridialog.h \
  optionsdialog.h \
  optionsmodel.h \
  overviewpage.h \
  paymentrequestplus.h \
  paymentserver.h \
  qvalidatedlineedit.h \
  qvaluecombobox.h \
  receivecoinsdialog.h \
  receiverequestdialog.h \
  recentrequeststablemodel.h \
  rpcconsole.h \
  sendcoinsdialog.h \
  sendcoinsentry.h \
  signverifymessagedialog.h \
  splashscreen.h \
  trafficgraphwidget.h \
  transactiondesc.h \
  transactiondescdialog.h \
  transactionfilterproxy.h \
  transactionrecord.h \
  transactiontablemodel.h \
  transactionview.h \
  utilitydialog.h \
  walletframe.h \
  walletmodel.h \
  walletmodeltransaction.h \
  walletview.h \
  winshutdownmonitor.h

RES_ICONS = \
  res/icons/add.png \
  res/icons/address-book.png \
  res/icons/ticoin.ico \
  res/icons/ticoin.png \
  res/icons/ticoin_testnet.ico \
  res/icons/ticoin_testnet.png \
  res/icons/clock1.png \
  res/icons/clock2.png \
  res/icons/clock3.png \
  res/icons/clock4.png \
  res/icons/clock5.png \
  res/icons/configure.png \
  res/icons/connect0_16.png \
  res/icons/connect1_16.png \
  res/icons/connect2_16.png \
  res/icons/connect3_16.png \
  res/icons/connect4_16.png \
  res/icons/debugwindow.png \
  res/icons/edit.png \
  res/icons/editcopy.png \
  res/icons/editpaste.png \
  res/icons/export.png \
  res/icons/filesave.png \
  res/icons/history.png \
  res/icons/key.png \
  res/icons/lock_closed.png \
  res/icons/lock_open.png \
  res/icons/overview.png \
  res/icons/qrcode.png \
  res/icons/quit.png \
  res/icons/receive.png \
  res/icons/remove.png \
  res/icons/send.png \
  res/icons/synced.png \
  res/icons/toolbar.png \
  res/icons/toolbar_testnet.png \
  res/icons/transaction0.png \
  res/icons/transaction2.png \
  res/icons/transaction_conflicted.png \
  res/icons/tx_inout.png \
  res/icons/tx_input.png \
  res/icons/tx_output.png \
  res/icons/tx_mined.png

ticoin_QT_CPP = \
  ticoin.cpp \
  ticoinaddressvalidator.cpp \
  ticoinamountfield.cpp \
  ticoingui.cpp \
  ticoinunits.cpp \
  clientmodel.cpp \
  csvmodelwriter.cpp \
  guiutil.cpp \
  intro.cpp \
  monitoreddatamapper.cpp \
  notificator.cpp \
  optionsdialog.cpp \
  optionsmodel.cpp \
  qvalidatedlineedit.cpp \
  qvaluecombobox.cpp \
  rpcconsole.cpp \
  splashscreen.cpp \
  trafficgraphwidget.cpp \
  utilitydialog.cpp \
  winshutdownmonitor.cpp

if ENABLE_WALLET
ticoin_QT_CPP += \
  addressbookpage.cpp \
  addresstablemodel.cpp \
  askpassphrasedialog.cpp \
  coincontroldialog.cpp \
  coincontroltreewidget.cpp \
  editaddressdialog.cpp \
  openuridialog.cpp \
  overviewpage.cpp \
  paymentrequestplus.cpp \
  paymentserver.cpp \
  receivecoinsdialog.cpp \
  receiverequestdialog.cpp \
  recentrequeststablemodel.cpp \
  sendcoinsdialog.cpp \
  sendcoinsentry.cpp \
  signverifymessagedialog.cpp \
  transactiondesc.cpp \
  transactiondescdialog.cpp \
  transactionfilterproxy.cpp \
  transactionrecord.cpp \
  transactiontablemodel.cpp \
  transactionview.cpp \
  walletframe.cpp \
  walletmodel.cpp \
  walletmodeltransaction.cpp \
  walletview.cpp
endif

RES_IMAGES = \
  res/images/about.png \
  res/images/splash.png \
  res/images/splash_testnet.png

RES_MOVIES = $(wildcard res/movies/spinner-*.png)

ticoin_RC = res/ticoin-qt-res.rc

libticoinqt_a_CPPFLAGS = $(AM_CPPFLAGS) $(QT_INCLUDES) \
  -I$(top_srcdir)/src/qt/forms $(QT_DBUS_INCLUDES)
libticoinqt_a_SOURCES = $(ticoin_QT_CPP) $(ticoin_QT_H) $(QT_FORMS_UI) \
  $(QT_QRC) $(QT_TS) $(PROTOBUF_PROTO) $(RES_ICONS) $(RES_IMAGES) $(RES_MOVIES)

nodist_libticoinqt_a_SOURCES = $(QT_MOC_CPP) $(QT_MOC) $(PROTOBUF_CC) \
  $(PROTOBUF_H) $(QT_QRC_CPP)

BUILT_SOURCES = $(nodist_libticoinqt_a_SOURCES)

#Generating these with a half-written protobuf header leads to wacky results.
#This makes sure it's done.
$(QT_MOC): $(PROTOBUF_H)
$(QT_MOC_CPP): $(PROTOBUF_H)

# ticoin-qt binary #
ticoin_qt_CPPFLAGS = $(AM_CPPFLAGS) $(QT_INCLUDES) \
  -I$(top_srcdir)/src/qt/forms
ticoin_qt_SOURCES = ticoin.cpp
if TARGET_DARWIN
  ticoin_qt_SOURCES += $(ticoin_MM)
endif
if TARGET_WINDOWS
  ticoin_qt_SOURCES += $(ticoin_RC)
endif
ticoin_qt_LDADD = libticoinqt.a $(LIBticoin_SERVER)
if ENABLE_WALLET
ticoin_qt_LDADD += $(LIBticoin_WALLET)
endif
ticoin_qt_LDADD += $(LIBticoin_CLI) $(LIBticoin_COMMON) $(LIBLEVELDB) $(LIBMEMENV) \
  $(BOOST_LIBS) $(QT_LIBS) $(QT_DBUS_LIBS) $(QR_LIBS) $(PROTOBUF_LIBS) $(BDB_LIBS)
ticoin_qt_LDFLAGS = $(QT_LDFLAGS)

# forms/foo.h -> forms/ui_foo.h
QT_FORMS_H=$(join $(dir $(QT_FORMS_UI)),$(addprefix ui_, $(notdir $(QT_FORMS_UI:.ui=.h))))

#locale/foo.ts -> locale/foo.qm
QT_QM=$(QT_TS:.ts=.qm)

.PHONY: FORCE
.SECONDARY: $(QT_QM)

ticoinstrings.cpp: FORCE
	$(MAKE) -C $(top_srcdir)/src qt/ticoinstrings.cpp

translate: ticoinstrings.cpp $(QT_FORMS_UI) $(QT_FORMS_UI) $(ticoin_QT_CPP) $(ticoin_QT_H) $(ticoin_MM)
	@test -n $(LUPDATE) || echo "lupdate is required for updating translations"
	@QT_SELECT=$(QT_SELECT) $(LUPDATE) $^ -locations relative -no-obsolete -ts locale/ticoin_en.ts

$(QT_QRC_CPP): $(QT_QRC) $(QT_QM) $(QT_FORMS_H) $(RES_ICONS) $(RES_IMAGES) $(RES_MOVIES) $(PROTOBUF_H)
	@cd $(abs_srcdir); test -f $(RCC) && QT_SELECT=$(QT_SELECT) $(RCC) -name ticoin -o $(abs_builddir)/$@ $< || \
	  echo error: could not build $@
	$(SED) -e '/^\*\*.*Created:/d' $@ > $@.n && mv $@{.n,}
	$(SED) -e '/^\*\*.*by:/d' $@  > $@.n && mv $@{.n,}

CLEANFILES = $(BUILT_SOURCES) $(QT_QM) $(QT_FORMS_H) *.gcda *.gcno
