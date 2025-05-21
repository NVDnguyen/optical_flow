import cv2
import numpy as np
import argparse

# Function to calculate direction and average vector between two images
def calculate_direction(image_path1, image_path2):
    # Read two images from the paths
    img1 = cv2.imread(image_path1)
    img2 = cv2.imread(image_path2)

    # Check if the images are valid
    if img1 is None or img2 is None:
        print("Unable to read one or both images.")
        return

    # Convert to grayscale
    gray1 = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
    gray2 = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)

    # Find feature points in the first image
    p0 = cv2.goodFeaturesToTrack(gray1, maxCorners=100, qualityLevel=0.3, minDistance=7)

    # Parameters for Lucas-Kanade optical flow
    lk_params = dict(winSize=(15, 15), maxLevel=2,
                     criteria=(cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 0.03))

    p1, st, err = cv2.calcOpticalFlowPyrLK(gray1, gray2, p0, None, **lk_params)
    vectors = []
    mean_angle = None

    if p1 is not None:
        good_new = p1[st == 1]
        good_old = p0[st == 1]

        for (new, old) in zip(good_new, good_old):
            a, b = new.ravel()
            c, d = old.ravel()
            dx, dy = a - c, b - d
            vectors.append((dx, dy))

        if vectors:
            dxs, dys = zip(*vectors)
            mean_dx = np.mean(dxs)
            mean_dy = np.mean(dys)
            mean_angle = np.arctan2(-mean_dy, -mean_dx) * 180 / np.pi
            magnitude = np.sqrt(mean_dx**2 + mean_dy**2)

            print(f"Average vector: dx = {mean_dx:.2f}, dy = {mean_dy:.2f}")
            print(f"Average direction: {mean_angle:.2f} degrees")
            print(f"Vector magnitude: {magnitude:.2f}")

            # Draw the average vector from the center of the image
            h, w = img2.shape[:2]
            center = (w // 2, h // 2)
            endpoint = (int(center[0] + mean_dx * 10), int(center[1] + mean_dy * 10))
            cv2.arrowedLine(img2, center, endpoint, (255, 0, 0), 3)
            cv2.putText(img2, f"{mean_angle:.1f} deg", (10, 30),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 0, 0), 2)

    # Display direction
    if mean_angle is not None:
        if mean_angle < 0:
            print('=> Up')
        elif mean_angle > 0:
            print('=> Down')
        else:
            print('Unknown')

        cv2.namedWindow('Resulting Image with Vector', cv2.WINDOW_NORMAL)
        cv2.imshow('Resulting Image with Vector', img2)
        cv2.waitKey(0)  
        cv2.destroyAllWindows()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Calculate direction between two images.")
    parser.add_argument("image1", type=str, help="Path to the first image.")
    parser.add_argument("image2", type=str, help="Path to the second image.")
    args = parser.parse_args()

    # Call the function with the image paths
    calculate_direction(args.image1, args.image2)
