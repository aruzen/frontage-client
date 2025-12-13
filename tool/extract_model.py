import bpy
import sys

argv = sys.argv
argv = argv[argv.index("--") + 1:]
input_path = argv[0]
target_name = argv[1]
output_path = argv[2]

# すべてのオブジェクトを削除
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete()

# glTF を読み込み
bpy.ops.import_scene.gltf(filepath=input_path)

# 対象だけ残して他は削除
for obj in bpy.data.objects:
    if obj.name != target_name:
        obj.select_set(True)
    else:
        obj.select_set(False)

bpy.ops.object.delete()

# 対象オブジェクトだけエクスポート
bpy.ops.export_scene.gltf(filepath=output_path, export_format='GLTF_SEPARATE')
