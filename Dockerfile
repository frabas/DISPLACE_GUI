# Primo step: Builder
FROM ubuntu:22.04 AS builder

# Installazione dei pacchetti necessari per la compilazione e dpkg-buildpackage
RUN apt-get update && apt-get install -y \
    build-essential cmake git dpkg-dev debhelper devscripts equivs && \
    apt-get clean

# Clonazione del repository MSQLiteCpp da GitHub
WORKDIR /build
RUN git clone https://github.com/studiofuga/mSqliteCpp.git
RUN git clone https://github.com/greg7mdp/sparsepp.git

# Creazione dei pacchetti deb con dpkg-buildpackage
WORKDIR /build/mSqliteCpp
RUN mk-build-deps --install  --tool='apt-get -o Debug::pkgProblemResolver=yes --no-install-recommends --yes' ./debian/control
RUN dpkg-buildpackage -uc -us


# Installazione del pacchetto creato
WORKDIR /build
RUN apt-get install -y sqlite3
RUN dpkg -i *.deb

# Compilazione del progetto principale
WORKDIR /src
COPY . /src
RUN ln -s /build/sparsepp/sparsepp /usr/local/include/
RUN mk-build-deps --install  --tool='apt-get -o Debug::pkgProblemResolver=yes --no-install-recommends --yes' ./debian/control
RUN dpkg-buildpackage -uc -us

# Secondo step: Runtime
FROM ubuntu:22.04 AS runtime

# Installazione del pacchetto deb creato nel primo step
COPY --from=builder /build/*.deb /out/
RUN apt-get update && apt-get install -y dpkg && \
    dpkg -i /out/*.deb && \
    apt-get clean

# Copia del progetto compilato e esecuzione
WORKDIR /app
COPY --from=builder /src/build /app

# Comando di default
CMD ["./your_executable"] # Sostituisci "your_executable" con il nome del binario generato