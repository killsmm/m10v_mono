
FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"
SRC_URI += " \
			file://rtsp-client.c \
			file://rtsp-media.c \
			file://rtsp-media.h \
			file://rtsp-stream.h \
			file://rtsp-stream.c \
			file://rtsp_stream_com.h \
			file://rtsp-sdp.c \
"

S = "${WORKDIR}/${PNREAL}-${PV}/"
S2 = "gst/rtsp-server"

do_configure_append() {
    cp -p ${WORKDIR}/rtsp-client.c ${S}${S2}
	cp -p ${WORKDIR}/rtsp-media.c ${S}${S2}
	cp -p ${WORKDIR}/rtsp-media.h ${S}${S2}
    cp -p ${WORKDIR}/rtsp-stream.h ${S}${S2}
    cp -p ${WORKDIR}/rtsp-stream.c ${S}${S2}
    cp -p ${WORKDIR}/rtsp_stream_com.h ${S}${S2}
    cp -p ${WORKDIR}/rtsp-sdp.c ${S}${S2}

}

