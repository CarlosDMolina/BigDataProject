import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

# Before clustering
df = pd.read_csv("output.csv")
df.columns = ["j", "v","x"]
sns.scatterplot(x=df["j"],
                y=df["v"])

# After clustering
plt.figure()
df = pd.read_csv("output.csv")
sns.scatterplot(x=df.x, y=df.y,
                hue=df.c,
                palette=sns.color_palette("hls", n_colors=20))
plt.xlabel("j")
plt.ylabel("v")

plt.show()
