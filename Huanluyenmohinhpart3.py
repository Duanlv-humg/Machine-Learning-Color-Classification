import pandas as pd
import numpy as np
import random
import pickle
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.metrics import classification_report, confusion_matrix
from micromlgen import port

print("=" * 55)
print("        HUAN LUYEN AI 8 NHAN - DA SUA LOI DU LIEU")
print("=" * 55)

# ================================================================
#  1. DỮ LIỆU GỐC
# ================================================================
real_measurements = {
    "Red":    [(210, 40, 45),  (197, 26, 28),  (177, 33, 38)],
    "Blue":   [(28, 70, 143),  (20, 67, 157),  (40, 73, 137)],
    "Yellow": [(103, 100, 40), (111, 90, 53),  (117, 94, 41)],
    "Green":  [(45, 146, 53),  (55, 136, 53),  (65, 135, 51)],
    "Pink":   [(99, 55, 96),   (99, 54, 97),   (97, 53, 99)],
    "Purple": [(64, 57, 127),  (65, 56, 126),  (69, 53, 130)],
    "White":  [(235, 232, 238), (238, 235, 242), (233, 230, 236)],
    "Background": [(213, 205, 223), (215, 207, 225), (211, 203, 221)],
}

NUM_SYNTHETIC = 100

# ================================================================
#  KIỂM TRA CHẤT LƯỢNG DỮ LIỆU
# ================================================================
print("\n[0/5] Kiem tra chat luong du lieu...")
print(f"  {'Nhan':<12} {'Mau':<4} {'Bright_min':>12} {'Bright_max':>12} {'Chenh':>8} {'Ket qua':>10}")
print("  " + "-" * 64)

has_error = False
for label, measurements in real_measurements.items():
    brightnessvals = [(r+g+b)/(255*3) for (r,g,b) in measurements]
    bmin = min(brightnessvals)
    bmax = max(brightnessvals)
    chenh = bmax - bmin

    if chenh > 0.20:
        status = " CANH BAO"
        has_error = True
    elif chenh > 0.10:
        status = "⚠ Xem lai"
    else:
        status = " OK"

    print(f"  {label:<12} {len(measurements):<4} {bmin:>12.3f} {bmax:>12.3f} {chenh:>8.3f} {status:>10}")

if has_error:
    print("\n  [!] Co nhan bi o nhiem du lieu!")
else:
    print("\n  [OK] Tat ca nhan du lieu dong nhat. San sang huan luyen.")

# ================================================================
#  2. SINH DỮ LIỆU CÓ NHIỄU (ĐỒNG BỘ QUANG HỌC)
# ================================================================
print("\n[1/5] Dang sinh du lieu co nhieu...")
all_data = []

for label, measurements in real_measurements.items():
    for (base_r, base_g, base_b) in measurements:
        all_data.append([base_r, base_g, base_b, label])

    for (base_r, base_g, base_b) in measurements:
        for _ in range(NUM_SYNTHETIC):
            light_factor = random.uniform(0.4, 1.6)
            noise_r = random.gauss(0, 4)
            noise_g = random.gauss(0, 4)
            noise_b = random.gauss(0, 4)

            r = max(0, min(255, int((base_r * light_factor) + noise_r)))
            g = max(0, min(255, int((base_g * light_factor) + noise_g)))
            b = max(0, min(255, int((base_b * light_factor) + noise_b)))

            all_data.append([r, g, b, label])

df = pd.DataFrame(all_data, columns=['R', 'G', 'B', 'Label'])
df.to_csv("Color_dataset_8mau.csv", index=False)
# XUẤT FILE DATASET CSV RA MÁY TÍNH
print("  => Da luu dataset: Color_dataset_8mau.csv")
# ================================================================
#  3. TRÍCH XUẤT ĐẶC TRƯNG
# ================================================================
print("\n[2/5] Chuan bi dac trung...")
df['Sum']        = df['R'] + df['G'] + df['B'] + 1e-6
df['R_ratio']    = df['R'] / df['Sum']
df['G_ratio']    = df['G'] / df['Sum']
df['B_ratio']    = df['B'] / df['Sum']
df['Brightness'] = df['Sum'] / (255 * 3 + 1e-6)

X = df[['R_ratio', 'G_ratio', 'B_ratio', 'Brightness']].values
y = df['Label'].values

# ================================================================
#  4 & 5. TÁCH TẬP + TÌM max_depth TỐT NHẤT
# ================================================================
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.20, random_state=42, stratify=y)

print("\n[3/5] Tim max_depth tot nhat...")
best_depth    = 5
best_cv_score = 0.0
for depth in range(3, 12):
    clf_try = DecisionTreeClassifier(max_depth=depth, min_samples_split=8, min_samples_leaf=4, random_state=42)
    cv_scores = cross_val_score(clf_try, X_train, y_train, cv=5)
    mean = cv_scores.mean()
    if mean > best_cv_score:
        best_cv_score = mean
        best_depth    = depth

# ================================================================
#  6. HUẤN LUYỆN VÀ ĐÁNH GIÁ
# ================================================================
print(f"\n[4/5] Huan luyen voi max_depth = {best_depth}...")
clf = DecisionTreeClassifier(max_depth=best_depth, min_samples_split=8, min_samples_leaf=4, random_state=42)
clf.fit(X_train, y_train)

y_pred    = clf.predict(X_test)
train_acc = clf.score(X_train, y_train)
test_acc  = (y_pred == y_test).mean()

print(f"\n  Accuracy TRAIN : {train_acc*100:.1f}%")
print(f"  Accuracy TEST  : {test_acc*100:.1f}%")

print("\n  Bao cao chi tiet tung mau:")
print(classification_report(y_test, y_pred, zero_division=0))

labels_sorted = sorted(set(y))
cm = confusion_matrix(y_test, y_pred, labels=labels_sorted)
header = f"  {'':>12}" + "".join(f"{l[:6]:>8}" for l in labels_sorted)
print("\n  Confusion Matrix:")
print(header)
for i, row_label in enumerate(labels_sorted):
    row_str = f"  {row_label:>12}"
    for j in range(len(labels_sorted)):
        val = cm[i, j]
        if i == j:
            row_str += f"{'['+str(val)+']':>8}"
        elif val > 0:
            row_str += f"{'!'+str(val)+'!':>8}"
        else:
            row_str += f"{'·':>8}"
    print(row_str)

# ================================================================
#  7. XUẤT FILE 
# ================================================================
print("\n[5/5] Xuat file...")
with open('ColorModel_8mau.h', 'w') as f:
    f.write(port(clf))
with open('ColorModel_8mau.pkl', 'wb') as f:
    pickle.dump(clf, f)
print("  Hoan tat!")
