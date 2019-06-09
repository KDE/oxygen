file(GLOB schemefiles *.colors)
install( FILES ${schemefiles} DESTINATION 
${KDE_INSTALL_DATADIR}/color-schemes )
