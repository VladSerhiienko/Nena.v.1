-- Add exporter global variables

io.write("-- Running ", _VERSION, "\n")
io.write("-- Importing joints...\n")

exporter_config = { }
exporter_config.export_shapes = 0
exporter_config.export_skin_clusters = 0
exporter_config.export_joint_clusters = 1
exporter_config.export_transformations = 1
exporter_config.export_animation_curves = 0


