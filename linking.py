from imutils import paths
from spherecluster import SphericalKMeans
from utilities import *
import pandas as pd
import numpy as np

if os.path.exists(tracks_path+"/cooccurance.csv"):
	os.remove(tracks_path+"/cooccurance.csv")
if os.path.exists(tracks_path+"/embedding.csv"):
	os.remove(tracks_path+"/embedding.csv")


# Generate cooccurance matrix
# By reading csv file
folders = os.listdir(tracks_path)
csv_file = tracks_path+"/cooccurance.csv"
with open( csv_file, mode='a') as employee_file:
	employee_writer = csv.writer(employee_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
	employee_writer.writerow(['Speaker', 'First', 'Last'])


for folder in folders:
	print("[INFO] processing {} folder ...".format(folder))
	sort(tracks_path, folder, csv_file)
print("[INFO] Generating Co-occurance Matrix ...")

matrix = cooccurance_matrix(csv_file)

# load our serialized face embedding model from disk
print("[INFO] loading face recognizer...")
embedder = cv2.dnn.readNetFromTorch("PATH_TO_OPENFACE_FILE")


# grab the paths to the input images in our dataset
print("[INFO] quantifying faces...")
imagePaths = list(paths.list_images(tracks_path))

identities = []
vectors = []
frames = []

for (i, imagePath) in enumerate(imagePaths):
	identity = imagePath.split('/')[-2]
	frame = imagePath.split('/')[-1].split('_')[0][1:]
	if (i+1)%1000 == 0:
		print("[INFO] processing image {}/{}".format(i + 1, len(imagePaths)))
	
	image = cv2.imread(imagePath)
	
	faceBlob = cv2.dnn.blobFromImage(image, 1.0 / 255, (96, 96), (0, 0, 0), swapRB=True, crop=False)
	embedder.setInput(faceBlob)
	#vec = vgg_face_descriptor.predict(preprocess_image(imagePath))[0,:]
	vec = embedder.forward()
	vectors.append(vec.flatten())
	identities.append(int(identity[1:]))
	frames.append(int(frame))
	
identities = np.array(identities)
frames = np.array(frames)
vectors = np.array(vectors)
print(vectors.shape, frames.shape, identities.shape, len(imagePaths))

df = pd.DataFrame(vectors, columns = [str(k) for k in range(0,128)])
df['frames'] = frames
df['identities'] = identities
X = df.loc[:, '0':'127']
Y = df['identities']

print("[INFO] Finding cluster centroids ...")
skm = SphericalKMeans(n_clusters = 1000, verbose=1, n_jobs=-2, rangedom_state=1)
skm.fit(X)
labels = skm.predict(X)
df['labels'] = labels
df = df.sort_values(by='identities')

df.to_csv(tracks_path + "/embedding.csv", index=False)

print("[INFO] embeddings complete ...")
print("[INFO] Linking ...")
