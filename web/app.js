const sceneShapes = [];
const endpoint = "/api/scene";

const forms = {
    circle: document.querySelector("#circleForm"),
    rectangle: document.querySelector("#rectangleForm"),
    line: document.querySelector("#lineForm"),
    text: document.querySelector("#textForm")
};

const listElements = {
    circle: document.querySelector("#circleList"),
    rectangle: document.querySelector("#rectangleList"),
    line: document.querySelector("#lineList"),
    text: document.querySelector("#textList")
};

const scenePreview = document.querySelector("#scenePreview");
const sceneSummary = document.querySelector("#sceneSummary");
const statusMessage = document.querySelector("#statusMessage");
const sendSceneButton = document.querySelector("#sendSceneButton");
const clearSceneButton = document.querySelector("#clearSceneButton");
const matrixWidthInput = document.querySelector("#matrixWidth");
const matrixHeightInput = document.querySelector("#matrixHeight");

function numberValue(formData, name) {
    return Number(formData.get(name));
}

function scenePayload() {
    return { shapes: sceneShapes };
}

function renderScene() {
    const payload = scenePayload();

    scenePreview.textContent = JSON.stringify(payload, null, 2);
    sceneSummary.textContent = `${sceneShapes.length} ${sceneShapes.length === 1 ? "shape" : "shapes"}`;

    for (const listElement of Object.values(listElements)) {
        listElement.replaceChildren();
    }

    sceneShapes.forEach((shape, sceneIndex) => {
        const item = document.createElement("li");
        item.className = "shape-item";

        const details = document.createElement("code");
        details.textContent = shapeSummary(shape);

        const actions = document.createElement("div");
        actions.className = "shape-actions";

        const editButton = document.createElement("button");
        editButton.type = "button";
        editButton.className = "small-button";
        editButton.textContent = "Edit";
        editButton.addEventListener("click", () => editShape(sceneIndex));

        const deleteButton = document.createElement("button");
        deleteButton.type = "button";
        deleteButton.className = "small-button danger-button";
        deleteButton.textContent = "Delete";
        deleteButton.addEventListener("click", () => deleteShape(sceneIndex));

        actions.append(editButton, deleteButton);
        item.append(details, actions);
        listElements[shape.type].append(item);
    });
}

function shapeSummary(shape) {
    switch (shape.type) {
        case "circle":
            return `x=${shape.x}, y=${shape.y}, radius=${shape.radius}, color=${shape.color}, `
                + `filled=${shape.filled}, thickness=${shape.thickness}`;

        case "rectangle":
            return `x=${shape.x}, y=${shape.y}, width=${shape.width}, height=${shape.height}, `
                + `color=${shape.color}, filled=${shape.filled}, thickness=${shape.thickness}`;

        case "line":
            return `x1=${shape.x1}, y1=${shape.y1}, x2=${shape.x2}, y2=${shape.y2}, `
                + `color=${shape.color}, thickness=${shape.thickness}`;

        case "text":
            return `"${shape.text}", x=${shape.x}, y=${shape.y}, color=${shape.color}, `
                + `font=${shape.font}, fontSize=${shape.fontSize}`;

        default:
            return JSON.stringify(shape);
    }
}

function addShape(shape) {
    sceneShapes.push(shape);
    setStatus(`${shape.type[0].toUpperCase()}${shape.type.slice(1)} added.`, "success");
    renderScene();
}

function deleteShape(sceneIndex) {
    sceneShapes.splice(sceneIndex, 1);
    setStatus("Shape deleted.", "success");
    renderScene();
}

function editShape(sceneIndex) {
    const shape = sceneShapes[sceneIndex];
    const form = forms[shape.type];

    for (const [name, value] of Object.entries(shape)) {
        const input = form.elements.namedItem(name);

        if (!input) {
            continue;
        }

        if (input.type === "checkbox") {
            input.checked = value;
        } else {
            input.value = value;
        }
    }

    sceneShapes.splice(sceneIndex, 1);
    setStatus(`Editing ${shape.type}. Change values and click Add ${shape.type[0].toUpperCase()}${shape.type.slice(1)}.`, "success");
    renderScene();
    form.scrollIntoView({ behavior: "smooth", block: "center" });
}

function readCircle(formData) {
    return {
        type: "circle",
        x: numberValue(formData, "x"),
        y: numberValue(formData, "y"),
        radius: numberValue(formData, "radius"),
        color: formData.get("color"),
        filled: formData.get("filled") === "on",
        thickness: numberValue(formData, "thickness")
    };
}

function readRectangle(formData) {
    return {
        type: "rectangle",
        x: numberValue(formData, "x"),
        y: numberValue(formData, "y"),
        width: numberValue(formData, "width"),
        height: numberValue(formData, "height"),
        color: formData.get("color"),
        filled: formData.get("filled") === "on",
        thickness: numberValue(formData, "thickness")
    };
}

function readLine(formData) {
    return {
        type: "line",
        x1: numberValue(formData, "x1"),
        y1: numberValue(formData, "y1"),
        x2: numberValue(formData, "x2"),
        y2: numberValue(formData, "y2"),
        color: formData.get("color"),
        thickness: numberValue(formData, "thickness")
    };
}

function readText(formData) {
    return {
        type: "text",
        text: formData.get("text").trim(),
        x: numberValue(formData, "x"),
        y: numberValue(formData, "y"),
        color: formData.get("color"),
        font: formData.get("font").trim(),
        fontSize: numberValue(formData, "fontSize")
    };
}

function validateShape(shape) {
    const width = Number(matrixWidthInput.value);
    const height = Number(matrixHeightInput.value);

    if (!Number.isInteger(width) || !Number.isInteger(height) || width < 1 || height < 1) {
        return "Matrix width and height must both be positive whole numbers.";
    }

    const coordinateNames = ["x", "y", "x1", "y1", "x2", "y2"];

    for (const coordinateName of coordinateNames) {
        if (!(coordinateName in shape)) {
            continue;
        }

        const maximum = coordinateName.startsWith("x") ? width - 1 : height - 1;

        if (shape[coordinateName] < 0 || shape[coordinateName] > maximum) {
            return `${coordinateName} must be between 0 and ${maximum}.`;
        }
    }

    if (shape.type === "text" && !shape.text) {
        return "Text cannot be empty.";
    }

    return "";
}

function setStatus(message, kind = "") {
    statusMessage.textContent = message;
    statusMessage.className = `status-message ${kind}`;
}

for (const [type, form] of Object.entries(forms)) {
    form.addEventListener("submit", (event) => {
        event.preventDefault();

        const formData = new FormData(form);
        const readers = {
            circle: readCircle,
            rectangle: readRectangle,
            line: readLine,
            text: readText
        };

        const shape = readers[type](formData);
        const error = validateShape(shape);

        if (error) {
            setStatus(error, "error");
            return;
        }

        addShape(shape);
    });
}

clearSceneButton.addEventListener("click", () => {
    sceneShapes.length = 0;
    setStatus("Scene cleared in the editor. Send it to clear the physical display too.", "success");
    renderScene();
});

sendSceneButton.addEventListener("click", async () => {
    sendSceneButton.disabled = true;
    setStatus("Sending scene...");

    try {
        const response = await fetch(endpoint, {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify(scenePayload())
        });

        const responseText = await response.text();

        if (!response.ok) {
            throw new Error(responseText || `HTTP ${response.status}`);
        }

        setStatus(responseText || "Scene accepted by LED display.", "success");
    } catch (error) {
        setStatus(`Could not send scene: ${error.message}`, "error");
    } finally {
        sendSceneButton.disabled = false;
    }
});

renderScene();
